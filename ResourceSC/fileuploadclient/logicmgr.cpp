#include "logicmgr.h"
#include "global.h"
#include <QDebug>
#include "tcpclient.h"
#include <QDateTime>
#include <QJsonDocument>
#include <algorithm>

LogicMgr::~LogicMgr()
{
    _work_thread->quit();
    _work_thread->wait();
    _work_thread->deleteLater();
    _worker->deleteLater();
}

void LogicMgr::AddMD5File(const QString& fileMd5, const std::shared_ptr<QFileInfo>& fileInfo)
{
    std::lock_guard<std::mutex> lock(_file_mtx);
    _map_md5_files[fileMd5] = fileInfo;
}

// StartUpload: 在 MainWindow 发送初始窗口后调用，告知 LogicMgr 已发送了 initial_sent_count 个块
void LogicMgr::StartUpload(const QString& fileMd5, const std::shared_ptr<QFileInfo>& fileInfo, int initial_sent_count)
{
    std::lock_guard<std::mutex> lock(_file_mtx);
    _map_md5_files[fileMd5] = fileInfo;

    UploadState st;
    st.fileInfo = fileInfo;
    st.total_size = static_cast<int>(fileInfo->size());
    if (st.total_size % MAX_FILE_LEN) {
        st.last_seq = (st.total_size / MAX_FILE_LEN) + 1;
    }
    else {
        st.last_seq = st.total_size / MAX_FILE_LEN;
    }
    st.last_ack_seq = 0;
    st.next_seq_to_send = initial_sent_count + 1;
    st.in_flight = initial_sent_count;
    st.cwnd = std::max(1, initial_sent_count);
    st.ssthresh = 32;
    st.rtt_est_ms = 200.0;
    st.send_time.clear();

    // 注意：此函数并不再重复发送 initial 数据块，MainWindow 已发送初始块
    _upload_states[fileMd5] = std::move(st);
}

std::shared_ptr<QFileInfo> LogicMgr::GetFileInfo(const QString& fileMd5)
{
    std::lock_guard<std::mutex> lock(_file_mtx);
    auto it = _map_md5_files.find(fileMd5);
    if (it == _map_md5_files.end()) {
        return nullptr;
    }
    return it.value();
}

LogicMgr::LogicMgr() :QObject(nullptr)
{
    _worker = new LogicWorker();
    _work_thread = new QThread(this);
    //worker移动到子线程处理消息
    _worker->moveToThread(_work_thread);

    //连接信号和槽函数
    connect(this, &LogicMgr::sig_logic_process, _worker,
        &LogicWorker::slot_logic_process);
    connect(_worker, &LogicWorker::sig_trans_size,
        this, &LogicMgr::sig_trans_size);
    //启动线程
    _work_thread->start();
}

LogicWorker::LogicWorker(QObject* parent) :QObject(parent)
{
    InitHandlers();
}

void LogicWorker::InitHandlers()
{
    //注册测试消息
    _handlers[ID_TEST_MSG_RSP] = [this](QJsonObject obj) {
        auto err = obj["error"].toInt();
        if (err != RSP_SUCCESS) {
            qDebug() << "test msg rsp err is " << err;
            return;
        }

        auto data = obj["data"].toString();
        qDebug() << "recv data is " << data;
        };

    // 注册上传消息（带拥塞窗口自适应）
    _handlers[ID_UPLOAD_FILE_RSP] = [this](QJsonObject obj) {
        auto err = obj["error"].toInt();
        if (err != RSP_SUCCESS) {
            qDebug() << "upload msg rsp err is " << err;
            return;
        }

        auto name = obj["name"].toString();
        auto total_size = obj["total_size"].toInt();
        auto trans_size = obj["trans_size"].toInt();
        auto md5 = obj["md5"].toString();
        auto ack_seq = obj["seq"].toInt();

        qDebug() << "recv : " << name << " file trans_size is " << trans_size;
        emit sig_trans_size(trans_size);

        if (total_size == trans_size) {
            return;
        }

        // 获取上传状态和文件信息
        auto file_info = LogicMgr::Inst()->GetFileInfo(md5);
        if (!file_info) {
            qDebug() << "file info not found for md5" << md5;
            return;
        }

        // 访问并更新 UploadState（需要互斥）
        {
            std::lock_guard<std::mutex> lock(LogicMgr::Inst()->_file_mtx);
            auto it = LogicMgr::Inst()->_upload_states.find(md5);
            if (it == LogicMgr::Inst()->_upload_states.end()) {
                // 如果没有状态，初始化为只发送一个 ACK 后继续（兼容旧逻辑）
                UploadState st;
                st.fileInfo = file_info;
                st.total_size = total_size;
                if (st.total_size % MAX_FILE_LEN) {
                    st.last_seq = (st.total_size / MAX_FILE_LEN) + 1;
                }
                else {
                    st.last_seq = st.total_size / MAX_FILE_LEN;
                }
                st.last_ack_seq = ack_seq;
                st.next_seq_to_send = ack_seq + 1;
                st.in_flight = 0;
                st.cwnd = INIT_SEND_WINDOW;
                st.ssthresh = 32;
                st.rtt_est_ms = 200.0;
                LogicMgr::Inst()->_upload_states[md5] = st;
                it = LogicMgr::Inst()->_upload_states.find(md5);
            }

            UploadState& st = it.value();

            // 更新已确认 seq（如果服务器返回的是已确认的 seq）
            if (ack_seq > st.last_ack_seq) {
                // 计算 RTT：若存在发送时间记录
                qint64 now_ms = QDateTime::currentMSecsSinceEpoch();
                if (st.send_time.contains(ack_seq)) {
                    qint64 send_ms = st.send_time.value(ack_seq);
                    double sample_rtt = static_cast<double>(now_ms - send_ms);
                    // EMA 更新 RTT（alpha = 0.125）
                    st.rtt_est_ms = (1 - 0.125) * st.rtt_est_ms + 0.125 * sample_rtt;
                    // 清理记录
                    st.send_time.remove(ack_seq);
                }

                // 计算已经被确认的新块数
                int newly_acked = ack_seq - st.last_ack_seq;
                st.last_ack_seq = ack_seq;
                st.in_flight = std::max(0, st.in_flight - newly_acked);

                // 拥塞控制（AIMD）
                if (st.cwnd < st.ssthresh) {
                    // 慢启动：指数增长（每个 ACK 增长 1，简单实现近似加倍）
                    st.cwnd = std::min(MAX_CWND, st.cwnd + newly_acked);
                }
                else {
                    // 拥塞避免：每 RTT 增长 1，这里每个 ACK 增长 1/cwnd，累积近似为 +1/RTT
                    // 为实现简洁，这里用加法增 1 当收到 cwnd 个 ACK
                    static QMap<QString, int> ack_accum;
                    ack_accum[md5] += newly_acked;
                    if (ack_accum[md5] >= st.cwnd) {
                        st.cwnd = std::min(MAX_CWND, st.cwnd + 1);
                        ack_accum[md5] = 0;
                    }
                }
            }
            else {
                // 重复 ACK（没有新的 ack_seq），可以考虑快速恢复（这里简单实现计数）
                // 为简化，不实现 dup-ack 快速重传，但可在需时扩展
                // 如果 RTT 很大，视为拥塞，进行乘法减小
                qint64 now_ms = QDateTime::currentMSecsSinceEpoch();
                bool rtt_large = st.rtt_est_ms > 2000.0; // 经验阈值
                if (rtt_large) {
                    st.ssthresh = std::max(1, st.cwnd / 2);
                    st.cwnd = std::max(1, st.ssthresh);
                }
            }

            // 若检测到极端 RTT（样本很大），主动收缩窗口
            if (st.rtt_est_ms > 2000.0) {
                st.ssthresh = std::max(1, st.cwnd / 2);
                st.cwnd = std::max(1, st.ssthresh);
            }

            // 计算可发送的块数，填充窗口
            int can_send = st.cwnd - st.in_flight;
            while (can_send > 0 && st.next_seq_to_send <= st.last_seq && !LogicMgr::Inst()->Pause()) {
                // 组织并发送 next_seq_to_send 块
                QFile file(st.fileInfo->filePath());
                if (!file.open(QIODevice::ReadOnly)) {
                    qWarning() << "Could not open file:" << file.errorString();
                    break;
                }
                qint64 offset = static_cast<qint64>(st.next_seq_to_send - 1) * MAX_FILE_LEN;
                file.seek(offset);
                QByteArray buffer = file.read(MAX_FILE_LEN);
                if (buffer.isEmpty()) {
                    file.close();
                    break;
                }

                QJsonObject jsonObj;
                QString base64Data = buffer.toBase64();

                jsonObj["md5"] = md5;
                jsonObj["name"] = st.fileInfo->fileName();
                jsonObj["seq"] = st.next_seq_to_send;
                jsonObj["trans_size"] = static_cast<int>(offset + buffer.size());
                jsonObj["total_size"] = st.total_size;

                if (static_cast<int>(offset + buffer.size()) == st.total_size) {
                    jsonObj["last"] = 1;
                }
                else {
                    jsonObj["last"] = 0;
                }

                jsonObj["data"] = base64Data;
                jsonObj["last_seq"] = st.last_seq;
                QJsonDocument doc(jsonObj);
                auto send_data = doc.toJson();

                // 记录发送时间用于 RTT 估计
                st.send_time[st.next_seq_to_send] = QDateTime::currentMSecsSinceEpoch();

                TcpClient::Inst().sendMsg(ID_UPLOAD_FILE_REQ, send_data);

                st.in_flight++;
                st.next_seq_to_send++;
                can_send = st.cwnd - st.in_flight;
                file.close();
            }

            // 限制 cwnd 不超过 MAX_CWND
            if (st.cwnd > MAX_CWND) st.cwnd = MAX_CWND;
        } // end locked block
        };
}

void LogicWorker::slot_logic_process(quint16 msgid, QJsonObject obj)
{
    if (!_handlers.contains(msgid)) {
        qDebug() << "msgid: " << msgid << " not found";
        return;
    }
    _handlers[msgid](obj);
}

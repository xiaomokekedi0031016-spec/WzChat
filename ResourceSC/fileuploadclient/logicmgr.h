#ifndef LOGICMGR_H
#define LOGICMGR_H
#include "singleton.h"
#include "global.h"
#include <functional>
#include <QMap>
#include <QJsonObject>
#include <QObject>
#include <QThread>
#include <QFileInfo>
#include <memory>
#include <mutex>
#include <atomic>
#include <QMap>

class LogicWorker : public QObject {
    Q_OBJECT
public:
    LogicWorker(QObject* parent = nullptr);
    void InitHandlers();
private:
    QMap<quint32, std::function<void(QJsonObject obj)> >_handlers;
signals:
    void sig_trans_size(int trans_size);
public slots:
    void slot_logic_process(quint16 msgid, QJsonObject obj);
};

// 上传状态结构体（每个文件独立维护）
struct UploadState {
    std::shared_ptr<QFileInfo> fileInfo;
    int total_size{ 0 };
    int last_seq{ 0 };           // 总块数
    int last_ack_seq{ 0 };       // 已确认的最大 seq（ACK）
    int next_seq_to_send{ 1 };   // 下一要发送的 seq
    int in_flight{ 0 };          // 在飞块数（已发送未确认）
    int cwnd{ INIT_SEND_WINDOW }; // 拥塞窗口
    int ssthresh{ 32 };          // 慢启动阈值
    double rtt_est_ms{ 200.0 };  // RTT 估计（ms）
    QMap<int, qint64> send_time; // seq -> 发送时间 ms

    // 为实现快速重传/快速恢复与超时退避，增加下述字段
    int dup_ack_count{ 0 };      // 重复 ACK 计数
    bool in_fast_recovery{ false }; // 是否处于快速恢复
    int recover_seq{ 0 };        // 快速恢复退出阈值（按 RFC 规定通常为最高已发送 seq）
    int rto_ms{ 1000 };         // 当前 RTO（可基于 rtt_est_ms 调整）

    UploadState() = default;
};

//QObject 放在前面
class LogicMgr : public QObject, public Singleton<LogicMgr>
{
    Q_OBJECT
public:
    friend class Singleton<LogicMgr>;
    friend class LogicWorker; // 添加此行，使 LogicWorker 可以访问 LogicMgr 的私有成员
    ~LogicMgr();
    // 保留旧的文件信息添加（兼容）
    void AddMD5File(const QString& fileMd5, const std::shared_ptr<QFileInfo>& fileInfo);

    // 新：启动上传，告知 LogicMgr 已经由外部发送了 initial_sent_count 个块
    void StartUpload(const QString& fileMd5, const std::shared_ptr<QFileInfo>& fileInfo, int initial_sent_count);

    // 线程安全查询文件信息
    std::shared_ptr<QFileInfo> GetFileInfo(const QString& fileMd5);

    // 添加 Pause 方法声明
    bool Pause() const;
    // 设置暂停状态（线程安全）
    void SetPause(bool pause);

private:
    LogicMgr();
    LogicWorker* _worker;
    QThread* _work_thread;

    // 文件映射与互斥保护
    std::mutex _file_mtx;
    QMap<QString, std::shared_ptr<QFileInfo>> _map_md5_files;

    // 上传状态 map（md5 -> UploadState）
    QMap<QString, UploadState> _upload_states;

    // 线程安全的暂停标志
    mutable std::atomic_bool _paused{ false };

signals:
    void sig_logic_process(quint16 msgid, QJsonObject obj);
    void sig_trans_size(int trans_size);
};

// 添加 Pause / SetPause 方法实现（线程安全）
inline bool LogicMgr::Pause() const
{
    return _paused.load(std::memory_order_acquire);
}

inline void LogicMgr::SetPause(bool pause)
{
    _paused.store(pause, std::memory_order_release);
}

#endif // LOGICMGR_H

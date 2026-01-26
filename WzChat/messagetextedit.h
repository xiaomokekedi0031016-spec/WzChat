#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H

#include "global.h"

#include <QTextEdit>


class MessageTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MessageTextEdit(QWidget* parent = nullptr);
    ~MessageTextEdit();

    //获取当前输入框的所有信息
    //把QTextEdit里“混合的文字 + 图片”解析成一个有顺序的QVector<MsgInfo>消息列表
    QVector<MsgInfo> getMsgList();

protected:
    //拖拽进入控件时触发的事件
    void dragEnterEvent(QDragEnterEvent* event) override;
    //当拖拽操作在QTextEdit上释放鼠标时触发。
    void dropEvent(QDropEvent* event) override;
    //捕捉enter键盘按键发送send信号 >> 没写
    void keyPressEvent(QKeyEvent* e) override;
    //重写解析拖拽来的数据
    void insertFromMimeData(const QMimeData *source)override;

private:
    //把输入框里的消息插入到消息列表里
    void insertMsgList(QVector<MsgInfo> &list,
                       QString flag, QString text, QPixmap pix);
    //解析文本内容里的url
    QStringList getUrl(QString text);
    //判断拖拽来的文件是否为图片
    bool isImage(QString url);
    //插入图片
    void insertImages(const QString &url);
    //插入文件
    void insertTextFile(const QString &url);
    //获取文件图标及大小信息，并转化成图片
    QPixmap getFileIconPixmap(const QString &url);
    //获取文件大小
    QString getFileSize(qint64 size);
    // 重写判断是否能插入数据
    bool canInsertFromMimeData(const QMimeData *source) const;

signals:
    //发送消息信号
    void send();

private:
    QVector<MsgInfo> mMsgList;
    QVector<MsgInfo> mGetMsgList;//最终数据
};

#endif // MESSAGETEXTEDIT_H

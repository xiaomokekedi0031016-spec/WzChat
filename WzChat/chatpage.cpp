#include "chatitembase.h"
#include "chatpage.h"
#include "picturebubble.h"
#include "textbubble.h"
#include "ui_chatpage.h"
#include <QPainter>
#include <QStyleOption>
#include "chatview.h"
#include "global.h"


ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    ui->receive_btn->SetState("normal","hover","press");
    ui->send_btn->SetState("normal","hover","press");
    ui->emo_lb->SetState("normal","hover","press","normal","hover","press");
    ui->file_lb->SetState("normal","hover","press","normal","hover","press");
}

ChatPage::~ChatPage()
{
    delete ui;
}


void ChatPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);

    QPainter p(this);

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void ChatPage::on_send_btn_clicked()
{
    auto pTextEdit = ui->chatEdit;
    ChatRole role = ChatRole::Self;
    QString userName = QStringLiteral("badwoman");
    QString userIcon = ":/res/head_1.jpg";

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    for(int i=0; i<msgList.size(); ++i)
    {
        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;
        if(type == "text")
        {
            pBubble = new TextBubble(role, msgList[i].content);
            // qDebug() << "pTextEdit:" << pTextEdit;
            // qDebug() << "msgList size:" << msgList.size();

        }
        else if(type == "image")
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
        }
        else if(type == "file")
        {

        }
        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);//这里实际上隐含父子关系了
        }
    }
}


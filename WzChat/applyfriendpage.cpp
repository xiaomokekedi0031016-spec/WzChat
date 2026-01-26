#include "applyfriendpage.h"
#include "tcpmgr.h"
#include "ui_applyfriendpage.h"
#include "usermgr.h"
#include "userdata.h"

#include <QPainter>
#include <QRandomGenerator>

ApplyFriendPage::ApplyFriendPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ApplyFriendPage)
{
    ui->setupUi(this);
    connect(ui->apply_friend_list, &ApplyFriendList::sig_show_search,
            this, &ApplyFriendPage::sig_show_search);

    loadApplyList();
    //接受tcp传递的authrsp信号处理
    //connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp,
            //this, &ApplyFriendPage::slot_auth_rsp);
}

ApplyFriendPage::~ApplyFriendPage()
{
    delete ui;
}


void ApplyFriendPage::AddNewApply(std::shared_ptr<AddFriendApply> apply)
{

}


void ApplyFriendPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}



void ApplyFriendPage::loadApplyList()
{
    //模拟假数据，创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();
        auto *apply_item = new ApplyFriendItem();
        auto apply = std::make_shared<ApplyInfo>(0, names[name_i], strs[str_i],
                                                 heads[head_i], names[name_i], 0, 1);
        apply_item->SetInfo(apply);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui->apply_friend_list->addItem(item);
        ui->apply_friend_list->setItemWidget(item, apply_item);
        //收到审核好友信号
        // connect(apply_item, &ApplyFriendItem::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info){
        //                auto *authFriend =  new AuthenFriend(this);
        //                authFriend->setModal(true);
        //                authFriend->SetApplyInfo(apply_info);
        //                authFriend->show();
        // });
    }
}


#include "chatdialog.h"
#include "chatuserwid.h"
#include "loadingdlg.h"
#include "ui_chatdialog.h"
#include <QAction>//QLineEdit内嵌QAction
#include <QRandomGenerator>

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
    , _mode(ChatUIMode::ChatMode)
    , _state(ChatUIMode::ChatMode)
    , _b_loading(false)
{
    ui->setupUi(this);
    ui->add_btn->SetState("normal", "hover", "press");
    ui->search_edit->SetMaxLength(15);
    //搜索动作
    QAction *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/res/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));
    //清除动作
    QAction *clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/res/close_transparent.png"));
    //设置透明图标>>技巧
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);
    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text) {
        if (!text.isEmpty()) {
            clearAction->setIcon(QIcon(":/res/close_search.png"));
        } else {
            clearAction->setIcon(QIcon(":/res/close_transparent.png")); // 文本为空时，切换回透明图标
        }
    });

    connect(clearAction, &QAction::triggered, [this, clearAction]() {
        ui->search_edit->clear();//清空文字
        clearAction->setIcon(QIcon(":/res/close_transparent.png")); // 清除文本后，切换回透明图标
        //让控件失去焦点，不再接收键盘输入
        ui->search_edit->clearFocus();//失去焦点会调用失去焦点事件处理函数QWidget自带的
        //清除按钮被按下则不显示搜索框
        ShowSearch(false);
    });
    ShowSearch(false);
    connect(ui->chat_user_list,&ChatUserList::sig_loading_chat_user,
            this,&ChatDialog::slot_loading_chat_user);
    addChatUserList();

    QPixmap pixmap(":/res/ymjn.jpg");
    QPixmap scaledPixmap = pixmap.scaled( ui->side_head_lb->size(), Qt::KeepAspectRatio);
    ui->side_head_lb->setPixmap(scaledPixmap);
    ui->side_head_lb->setScaledContents(true);
    ui->side_chat_lb->setProperty("state","normal");
    ui->side_chat_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_contact_lb->setProperty("state","normal");
    ui->side_contact_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");

    ui->side_chat_lb->SetSelected(true);
    ui->search_list->SetSearchEdit(ui->search_edit);

    AddLBGroup(ui->side_chat_lb);
    AddLBGroup(ui->side_contact_lb);

    //聊天侧边栏
    connect(ui->side_chat_lb, &StateWidget::clicked,
            this, &ChatDialog::slot_side_chat);
    //联系人侧边栏
    connect(ui->side_contact_lb, &StateWidget::clicked,
            this, &ChatDialog::slot_side_contact);

    //链接搜索框输入变化
    connect(ui->search_edit, &QLineEdit::textChanged,
            this, &ChatDialog::slot_text_changed);

    //连接加载联系人的信号和槽函数
    connect(ui->con_user_list, &ContactUserList::sig_loading_contact_user,
            this, &ChatDialog::slot_loading_contact_user);

    //连接点击联系人item发出的信号和用户信息展示槽函数
    connect(ui->con_user_list, &ContactUserList::sig_switch_friend_info_page,
            this, &ChatDialog::slot_friend_info_page);

    //连接联系人页面点击好友申请条目的信号
    connect(ui->con_user_list, &ContactUserList::sig_switch_apply_friend_page,
            this, &ChatDialog::slot_switch_apply_friend_page);

    this->installEventFilter(this);

}

ChatDialog::~ChatDialog()
{
    delete ui;
}



void ChatDialog::ShowSearch(bool bsearch)
{
    if(bsearch){
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        _mode = ChatUIMode::SearchMode;
    }else if(_state == ChatUIMode::ChatMode){
        ui->chat_user_list->show();
        ui->con_user_list->hide();
        ui->search_list->hide();
        _mode = ChatUIMode::ChatMode;
    }else if(_state == ChatUIMode::ContactMode){
        ui->chat_user_list->hide();
        ui->search_list->hide();
        ui->con_user_list->show();
        _mode = ChatUIMode::ContactMode;
    }
}



void ChatDialog::slot_loading_chat_user()
{
    if(_b_loading){
        return;
    }
    _b_loading = true;
    LoadingDlg *loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    loadMoreChatUser();
    loadingDialog->deleteLater();
    _b_loading = false;
}



void ChatDialog::addChatUserList()
{
    for(int i = 0; i < 13; ++i){
        int randomValue = QRandomGenerator::global()->bounded(100);
        int str_i = randomValue % strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();
        auto* chat_user_wid = new ChatUserWid();
        auto user_info = std::make_shared<UserInfo>(0,names[name_i],names[name_i],
                                                    heads[head_i],0,strs[str_i]);
        chat_user_wid->SetInfo(user_info);
        QListWidgetItem* item = new QListWidgetItem;
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }
}



void ChatDialog::loadMoreChatUser()
{
    for(int i = 0; i < 13; ++i){
        int randomValue = QRandomGenerator::global()->bounded(100);
        int str_i = randomValue % strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();
        auto* chat_user_wid = new ChatUserWid();
        auto user_info = std::make_shared<UserInfo>(0,names[name_i],names[name_i],
                                                    heads[head_i],0,strs[str_i]);
        chat_user_wid->SetInfo(user_info);
        QListWidgetItem* item = new QListWidgetItem;
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }
}



void ChatDialog::slot_side_chat()
{
    ClearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    _state = ChatUIMode::ChatMode;
    ShowSearch(false);
}



void ChatDialog::slot_side_contact()
{
    ClearLabelState(ui->side_contact_lb);
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
    _state = ChatUIMode::ContactMode;
    ShowSearch(false);
}



void ChatDialog::ClearLabelState(StateWidget *lb)
{
    for(auto & ele: _lb_list){
        if(ele == lb){
            continue;
        }

        ele->ClearState();
    }
}



void ChatDialog::AddLBGroup(StateWidget *lb)
{
    _lb_list.push_back(lb);
}



void ChatDialog::slot_text_changed(const QString &str)
{
    if(!str.isEmpty()){
        ShowSearch(true);
    }
}



bool ChatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress){
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        //处理鼠标事件
        handleGlobalMousePress(mouseEvent);

    }
    return QDialog::eventFilter(watched,event);
}



void ChatDialog::handleGlobalMousePress(QMouseEvent *event)
{
    if( _mode != ChatUIMode::SearchMode){
        return;
    }
    // 将鼠标点击位置转换为搜索列表坐标系中的位置
    QPoint posInSearchList = ui->search_list->mapFrom(this, event->pos());
    if (!ui->search_list->rect().contains(posInSearchList)) {
        // 如果不在聊天列表内，清空输入框
        ui->search_edit->clear();
        ShowSearch(false);
    }
}

void ChatDialog::slot_loading_contact_user()
{
    //qDebug() << "slot loading contact user";
    if(_b_loading){
        return;
    }

    _b_loading = true;
    LoadingDlg *loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    //qDebug() << "add new data to list.....";
    loadMoreConUser();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    _b_loading = false;
}

void ChatDialog::loadMoreConUser(){
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();

        auto *con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(0,names[name_i], heads[head_i]);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(con_user_wid->sizeHint());
        ui->con_user_list->addItem(item);
        ui->con_user_list->setItemWidget(item, con_user_wid);
    }
}

void ChatDialog::slot_friend_info_page(std::shared_ptr<UserInfo> user_info)
{
    //qDebug()<<"receive switch friend info page sig";
    _last_widget = ui->friend_info_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(user_info);
}


void ChatDialog::slot_switch_apply_friend_page()
{
    //qDebug()<<"receive switch apply friend page sig";
    _last_widget = ui->friend_apply_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
}

#include "searchlist.h"

#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QJsonObject>
#include <QJsonDocument>
#include "adduseritem.h"
#include "customizeedit.h"
#include "findfaildlg.h"
#include "findsuccessdlg.h"
#include "loadingdlg.h"
#include "tcpmgr.h"
#include "usermgr.h"

SearchList::SearchList(QWidget *parent)
    :QListWidget(parent)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->viewport()->installEventFilter(this);
    connect(this, &QListWidget::itemClicked,
            this, &SearchList::slot_item_clicked);
    addTipItem();
    // 连接搜索条目
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search,
           this, &SearchList::slot_user_search);
}



bool SearchList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter) {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    if(watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15;

        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        return true;
    }
    return QListWidget::eventFilter(watched, event);
}



void SearchList::addTipItem()
{
    //预留控件
    auto* invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;
    item_tmp->setSizeHint(QSize(250,10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);

    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}



void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    qDebug()<<"111111111111111";
    QWidget *widget = this->itemWidget(item); //获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }
    auto itemType = customItem->GetItemType();//获取item类型
    if(itemType == ListItemType::INVALID_ITEM){//不可点击的item
        qDebug()<<"slot invalid item clicked ";
        return;
    }
    //如果是提示添加用户
    if(itemType == ListItemType::ADD_USER_TIP_ITEM){
#if 0
        //模拟
        // auto si = std::make_shared<SearchInfo>(0,"wz","wz","hello , my friend!",0, ":/res/head_2.jpg");
        // m_findSuccessDlg = new FindSuccessDlg(this);
        // m_findSuccessDlg->SetSearchInfo(si);
        // m_findSuccessDlg->show();

        _find_dlg = std::make_shared<FindSuccessDlg>(this);
        //std::shared_ptr
        //_find_dlg = std::shared_ptr<FindSuccessDlg>(new FindSuccessDlg(this));
        auto si = std::make_shared<SearchInfo>(0,"wz","wz","hello , my friend!",0,"");
        (std::dynamic_pointer_cast<FindSuccessDlg>(_find_dlg))->SetSearchInfo(si);
        _find_dlg->show();
#endif
        if(_send_pending){
            return;
        }
        if(!_search_edit){
            return;
        }

        //waitPending(true);

        auto search_edit = dynamic_cast<CustomizeEdit*>(_search_edit);
        auto uid_str = search_edit->text();
        QJsonObject jsonObj;
        jsonObj["uid"] = uid_str;
        QJsonDocument doc(jsonObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        //chat server发送搜索请求tcp
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_SEARCH_USER_REQ,
                                                  jsonData);

        return;
    }
    CloseFindDlg();
}

void SearchList::waitPending(bool pending)
{
    if(pending){
        //qDebug()<<"11111111111111111111";
        _loadingDialog = new LoadingDlg(this);
        _loadingDialog->setModal(true);
        _loadingDialog->show();
        _send_pending = pending;
    }else{
        _loadingDialog->hide();
        _loadingDialog->deleteLater();
        _send_pending = pending;
    }
}

void SearchList::CloseFindDlg()
{
    if(_find_dlg){
        _find_dlg->hide();
        _find_dlg = nullptr;
    }
}

void SearchList::SetSearchEdit(QWidget *edit)
{
    _search_edit = edit;
}



void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
    waitPending(false);
    if(si == nullptr){
        _find_dlg = std::make_shared<FindFailDlg>(this);
    }
    else{
        auto self_uid = UserMgr::GetInstance()->GetUid();
        if(si->_uid == self_uid){
            return;
        }
        //...todo
        _find_dlg = std::make_shared<FindSuccessDlg>(this);
        std::dynamic_pointer_cast<FindSuccessDlg>(_find_dlg)->SetSearchInfo(si);
    }

    _find_dlg->show();
}


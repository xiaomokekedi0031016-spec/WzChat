//#include "applyfriend.h"
#include "applyfriend.h"
#include "findsuccessdlg.h"
#include "ui_findsuccessdlg.h"
#include <QDebug>
#include <QDir>

FindSuccessDlg::FindSuccessDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindSuccessDlg)
    ,_parent(parent)
{
    ui->setupUi(this);
    // 设置对话框标题
    setWindowTitle("添加");
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    //qDebug()<<"app_path:"<<app_path;
    QString pix_path = QDir::toNativeSeparators(app_path +
                                                QDir::separator() + "static"+QDir::separator()+"head_1.jpg");

    //qDebug()<<"pix_path:"<<pix_path;
    QPixmap head_pix(pix_path);
    //缩放图片
    head_pix = head_pix.scaled(ui->head_lb->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);
    ui->add_friend_btn->SetState("normal","hover","press");
    //不关闭这个窗口，用户无法操作主窗口
    this->setModal(true);
}

FindSuccessDlg::~FindSuccessDlg()
{
    qDebug()<<"FindSuccessDlg destruct";
    delete ui;
}

void FindSuccessDlg::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    ui->name_lb->setText(si->_name);
    _si = si;
}

// void FindSuccessDlg::on_add_friend_btn_clicked()
// {
//     //todo... 添加好友界面弹出
// }

void FindSuccessDlg::on_add_friend_btn_clicked()
{
    this->hide();
    //弹出加好友界面
    auto applyFriend = new ApplyFriend(_parent);
    applyFriend->SetSearchInfo(_si);
    applyFriend->setModal(true);
    applyFriend->show();
}

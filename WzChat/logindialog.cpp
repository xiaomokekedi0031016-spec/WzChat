#include "logindialog.h"
#include "ui_logindialog.h"

#include <QPainter>
#include <QPainterPath>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    initHead();

    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);

}

LoginDialog::~LoginDialog()
{
    delete ui;
}


void LoginDialog::initHead()
{
    QPixmap originalPixmap(":/res/ymjn.jpg");
    originalPixmap = originalPixmap.scaled(ui->head_label->size(),
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent);//透明

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);//抗锯齿
    painter.setRenderHint(QPainter::SmoothPixmapTransform);//平滑缩放

    QPainterPath path;
    //取图片宽和高中较小的那个值作为圆的直径
    int side = qMin(originalPixmap.width(), originalPixmap.height());
    path.addEllipse((originalPixmap.width() - side) / 2,
                    (originalPixmap.height() - side) / 2,
                    side, side);

    painter.setClipPath(path);//把QPainter的绘制区域限定在path里
    painter.drawPixmap(0, 0, originalPixmap);//把整张图片画出来,但只有圆形范围会生效

    ui->head_label->setPixmap(roundedPixmap);
}


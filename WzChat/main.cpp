#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <qDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();//关闭文件
    }else{
        qDebug("Open failed");
    }

    //配置文件
    QString app_path = QCoreApplication::applicationDirPath();
    QString fileName = "config.ini";
    QString config_path = QDir::toNativeSeparators(app_path +
                                                   QDir::separator() + fileName);
    qDebug() << "config_path =" << config_path;

    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://"+gate_host+":"+gate_port;

    MainWindow w;
    w.show();
    return a.exec();
}

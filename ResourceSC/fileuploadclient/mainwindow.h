#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initUI();
    void initSlots();
    void startDelay(int msecs);
private slots:
    void on_fileBtn_clicked();

    void on_testBtn_clicked();

    void on_conBtn_clicked();

    void on_uploadBtn_clicked();

    void slot_pause_continue();

private:
    enum UploadState {
        INIT,
        CONTINUE,
        PAUSE
    };
    UploadState _cur_state = INIT;
    Ui::MainWindow *ui;
    QString _file_name;
    QString _file_md5;
    bool _b_pause = false; // 添加此成员变量用于暂停/继续状态
};

#endif // MAINWINDOW_H

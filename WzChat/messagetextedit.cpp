#include "messagetextedit.h"

#include <QDragEnterEvent>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>

MessageTextEdit::MessageTextEdit(QWidget *parent)
    :QTextEdit(parent)
{
    this->setMaximumHeight(60);
}



MessageTextEdit::~MessageTextEdit()
{

}



QVector<MsgInfo> MessageTextEdit::getMsgList()
{
    mGetMsgList.clear();
    QString doc = this->document()->toPlainText();//获取纯文本形式的数据
    QString text="";//存储文本信息
    int indexUrl = 0;//遍历图片消息
    int count = mMsgList.size();
    for(int index = 0; index < doc.size(); ++index){
        if(doc[index] == QChar::ObjectReplacementCharacter){
            //先处理图片前的文字
            if(!text.isEmpty()){
                QPixmap pix;//默认构造空的
                //把文字放入到消息列表里
                insertMsgList(mGetMsgList, "text", text, pix);
                text.clear();
            }
            while(indexUrl<count)
            {
                MsgInfo msg =  mMsgList[indexUrl];
                //Qt 把图片插入 QTextEdit 时，内部本质上是用 HTML <img> 标签存储图片的
                if(this->document()->toHtml().contains(msg.content,Qt::CaseSensitive))
                {
                    indexUrl++;
                    mGetMsgList.append(msg);
                    break;
                }
                indexUrl++;
            }
        }
        else{
            text.append(doc[index]);
        }
    }
    //处理最后一张图片后面的文字
    if(!text.isEmpty()){
        QPixmap pix;
        insertMsgList(mGetMsgList, "text", text, pix);
        text.clear();
    }
    mMsgList.clear();
    this->clear();
    return mGetMsgList;
}



void MessageTextEdit::insertMsgList(QVector<MsgInfo> &list, QString flag, QString text, QPixmap pix)
{
    MsgInfo msg;
    msg.msgFlag=flag;
    msg.content = text;
    msg.pixmap = pix;
    list.append(msg);
}



void MessageTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->source()==this)//文本框拖拽忽略
        event->ignore();
    else
        event->accept();
}



void MessageTextEdit::dropEvent(QDropEvent *event)
{
    insertFromMimeData(event->mimeData());//event->mimeData表示拖拽的文件/图片
    event->accept();
}



void MessageTextEdit::keyPressEvent(QKeyEvent *e)
{
    if((e->key()==Qt::Key_Enter||e->key()==Qt::Key_Return)
        && !(e->modifiers() & Qt::ShiftModifier))
    {
        emit send();
        return;
    }
    QTextEdit::keyPressEvent(e);
}


bool MessageTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

void MessageTextEdit::insertFromMimeData(const QMimeData *source)
{
    QStringList urls = getUrl(source->text());
    if(urls.isEmpty()){
        return;
    }
    foreach (QString url, urls){
        if(isImage(url))
            insertImages(url);//往控件插入图片
        else
            insertTextFile(url);//往控件插入文件
    }
}



QStringList MessageTextEdit::getUrl(QString text)
{
    //windows拖拽是file:///C:/Users/xxx/Pictures/test.png
    QStringList urls;
    if(text.isEmpty()) return urls;
    QStringList list = text.split("\n");
    foreach (QString url, list) {
        if(!url.isEmpty()){
            QStringList str = url.split("///");
            if(str.size()>=2)
                urls.append(str.at(1));
        }
    }
    return urls;
}



bool MessageTextEdit::isImage(QString url)
{
    QString imageFormat = "bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp";
    QStringList imageFormatList = imageFormat.split(",");
    QFileInfo fileInfo(url);
    QString suffix = fileInfo.suffix();//得到文件的后缀
    if(imageFormatList.contains(suffix,Qt::CaseInsensitive)){
        return true;
    }
    return false;
}



void MessageTextEdit::insertImages(const QString &url)
{
    QImage image(url);
    //按比例缩放图片
    if(image.width()>120||image.height()>80)
    {
        if(image.width()>image.height())
        {
            image =  image.scaledToWidth(120,Qt::SmoothTransformation);
        }
        else
            image = image.scaledToHeight(80,Qt::SmoothTransformation);
    }
    QTextCursor cursor = this->textCursor();//获取光标
    cursor.insertImage(image,url);
    insertMsgList(mMsgList,"image",url,QPixmap::fromImage(image));
}



void MessageTextEdit::insertTextFile(const QString &url)
{
    QFileInfo fileInfo(url);
    //禁止拖动文件夹和大文件
    if(fileInfo.isDir())
    {
        QMessageBox::information(this,"提示","只允许拖拽单个文件!");
        return;
    }
    if(fileInfo.size()>100*1024*1024)
    {
        QMessageBox::information(this,"提示","发送的文件大小不能大于100M");
        return;
    }
    //根据文件类型生成一个文件图标
    QPixmap pix = getFileIconPixmap(url);
}



QPixmap MessageTextEdit::getFileIconPixmap(const QString &url)
{
    QFileIconProvider provder;//获取系统默认的文件图标工具类
    QFileInfo fileinfo(url);//用于获取文件信息
    QIcon icon = provder.icon(fileinfo);//根据文件信息返回图片

    QString strFileSize = getFileSize(fileinfo.size());

    QFont font(QString("宋体"),10,QFont::Normal,false);
    QFontMetrics fontMetrics(font);//字体测量对象
    //文件名尺寸
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());
    //文件大小尺寸
    QSize FileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    int maxWidth = textSize.width() > FileSize.width() ? textSize.width() :FileSize.width();
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();


    QPainter painter;
    painter.begin(&pix);//之后的绘制操作都会画在pix
    // 文件图标
    QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40,40));
    painter.setPen(Qt::black);
    // 文件名称
    QRect rectText(50+10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    // 文件大小
    QRect rectFile(50+10, textSize.height()+5, FileSize.width(), FileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}



QString MessageTextEdit::getFileSize(qint64 size)
{
    QString Unit;
    double num;
    if(size < 1024){
        num = size;
        Unit = "B";
    }
    else if(size < 1024 * 1024){
        num = size / 1024.0;
        Unit = "KB";
    }
    else if(size <  1024 * 1024 * 1024){
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else{
        num = size / 1024.0 / 1024.0/ 1024.0;
        Unit = "GB";
    }
    return QString::number(num,'f',2) + " " + Unit;//保留两位小数
}


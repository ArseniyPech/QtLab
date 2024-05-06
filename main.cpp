#include <QApplication>
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QLabel>
#include <QFileDialog>
#include <QImage>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QMessageBox>
#include <QObject>
#include <QByteArray>
#include <QBuffer>
#include <QImage>
#include <QString>
#include <QPixmap>


class Widget : public QWidget
{
    QImage* my_image;
    QLabel* image_label;
    QMenu* file;
    QMenu* help;
    QMenuBar* menubar;
    QHBoxLayout* l;
    QVBoxLayout* layout;
    tcpserver* tcp;
    QLabel* lbl;





public:
    QString filename;
    QString get_filename();
    Widget(QWidget* parent = nullptr);
    ~Widget();
public slots:
    void begin_server_slot();
    void stop_server_slot();
    void open_image_slot();
    void exit_slot();

    void info_slot();
};

class tcpserver : public QObject

{

    Q_OBJECT

        QTcpServer* myserver;

    QTcpSocket* mysocket;

    bool is_started = false;




public:
    void set_file(QString&);
    QPixmap* pxmp;
    QString file;

    explicit tcpserver(QObject* parent = nullptr);

    bool started();

public slots:

    void slotNewConnection();

    void slotServerRead();

    void slotClientDisconnected();

signals:


};

Widget::Widget(QWidget* parent)
    : QWidget(parent)
{
    resize(400, 400);


    l = new QHBoxLayout();
    layout = new QVBoxLayout(this);
    lbl = new QLabel();
    image_label = new QLabel();
    //daclare the layers on wich the menu will be located

    menubar = new QMenuBar();
    file = new QMenu();
    help = new QMenu();
    // declare the menubar and two menues

    file->setTitle("File");
    help->setTitle("Help");
    //setting tittles to the menues

    menubar->addMenu(file);
    menubar->addMenu(help);
    //adding menues to th menubar

    l->addWidget(menubar);
    l->addWidget(lbl);
    l->addWidget(image_label);
    //adding layots




    layout->addLayout(l);



    QAction* begin_server = file->addAction("Run server");
    QAction* stop_server = file->addAction("Stop server");
    QAction* open_image = file->addAction("Open image");
    QAction* exit = file->addAction("Exit");
    QAction* info = help->addAction("Info");
    //declare menu options

    connect(begin_server, &QAction::triggered, this, &Widget::begin_server_slot);
    connect(stop_server, &QAction::triggered, this, &Widget::stop_server_slot);
    connect(open_image, &QAction::triggered, this, &Widget::open_image_slot);
    connect(exit, &QAction::triggered, this, &Widget::exit_slot);
    connect(info, &QAction::triggered, this, &Widget::info_slot);
    //connecting actions to the slots





}
void Widget::begin_server_slot() {

    if (!tcp->started()) {
        tcp = new tcpserver(this);//creating new server becuase it did not exist
        lbl->setText("server is starting");

    }
    else {
        lbl->setText("server has alreasy been started");
        //if server exists we do not need to create one
    }
}

void Widget::stop_server_slot() {
    if (!tcp->started()) {
        lbl->setText("server hasn't been started");
        // if server doesnt exist we do not need to stop it
    }
    else {
        delete tcp;
        //delete the exemplar of server
        lbl->setText("server was deleted");
    }
}

void Widget::open_image_slot() {
    filename = QFileDialog::getOpenFileName(this, tr("Open file"), "../", tr("Image Files (*.png *.jpg)"));
    tcp->set_file(filename);
    QPixmap* pix = new QPixmap(filename, "PNG");
    tcp->pxmp = pix;
    my_image = new QImage(filename, "PNG");
    image_label = new QLabel(this);
    image_label->setPixmap(QPixmap::fromImage(*my_image));
    image_label->setFixedSize(width(), height() * 2 / 3);
    l->addWidget(image_label);
    //firstly, getting path to the image (we can use only png or jpg images), secondly converting the image and change sizes
    //than adding the image to the main horizontal layout
}

void Widget::exit_slot() {
    close();
    //close the programm to exit
}

void Widget::info_slot() {
    QMessageBox* message = new QMessageBox();
    message->setText("Author: Pogorelov Ilia\n QMake version 3.1 Using Qt version 5.12.8 in /usr/lib/x86_64-linux-gnu");
    message->exec();
    //creating a message box with infromation about the author and qmake and Qt versions
}



Widget::~Widget()
{
}

tcpserver::tcpserver(QObject* parent) : QObject(parent)

{

    myserver = new QTcpServer(this);

    connect(myserver, &QTcpServer::newConnection, this, &tcpserver::slotNewConnection);

    if (!myserver->listen(QHostAddress::Any, 6000)) {

        is_started = false;

    }
    else {

        is_started = true;

    }
    //checking if server exists, if it does is_strated = true, if not, is_sterted = false


}


void tcpserver::slotNewConnection()

{

    mysocket = myserver->nextPendingConnection();

    //by default, after connecting to the server, display a message
    mysocket->write("Hello, World!!! I am echo server!\r\n");


    connect(mysocket, &QTcpSocket::readyRead, this, &tcpserver::slotServerRead);

    connect(mysocket, &QTcpSocket::disconnected, this, &tcpserver::slotClientDisconnected);
    //connecting actions of reading messages and closing server to slots

}

void tcpserver::set_file(QString& s) {
    file = s;
}

bool tcpserver::started() {

    return is_started;

}



void tcpserver::slotServerRead()

{
    //QImage image = QImage(file);
    QByteArray array;
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    pxmp->save(&buffer, "PNG");
    buffer.close();
    mysocket->write(array);
    //after clients apply image converts to QByteArray

}


void tcpserver::slotClientDisconnected()

{

    mysocket->close();

}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}

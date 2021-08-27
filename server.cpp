#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    //window setups
    this->setMinimumSize(360, 220);
    this->setMaximumSize(360, 220);

    //show local IP
    ui->IP->setStyleSheet("border: 1px solid green; color: green; background: silver;");
    ui->IP->setAlignment(Qt::AlignCenter);
    auto ips = QNetworkInterface().allAddresses();
    foreach (auto ip, ips)
    {
         if (ip.protocol() == QAbstractSocket::IPv4Protocol && ip != QHostAddress::LocalHost)
         {
             IP = ip.toString();
             ui->IP->setText(IP);
             break;
         }
    }


    QObject::connect(ui->OKButton, SIGNAL(clicked()), this, SLOT(OKButtonSlot()));
    QObject::connect(ui->CancelButton, SIGNAL(clicked()), this, SLOT(CancelButtonSlot()));
}

Server::~Server()
{
    delete ui;
}

void Server::CancelButtonSlot()
{
    this->close();
    MainWindow* mainwin = new MainWindow();
    mainwin->show();
}

void Server::OKButtonSlot()
{
    this->close();
    Board* board = new Board(nullptr, true, IP);
    board->show();
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("军棋");
    this->setMinimumSize(360, 420);
    this->setMaximumSize(360, 420);

    QObject::connect(ui->ServerButton, SIGNAL(clicked()), this, SLOT(ServerButtonSlot()));
    QObject::connect(ui->ClientButton, SIGNAL(clicked()), this, SLOT(ClientButtonSlot()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ServerButtonSlot()
{
    this->close();
    Server* server = new Server(nullptr);
    server->show();
}

void MainWindow::ClientButtonSlot()
{
    this->close();
    Client* client = new Client(nullptr);
    client->show();
}

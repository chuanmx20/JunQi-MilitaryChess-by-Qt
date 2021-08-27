#ifndef SERVER_H
#define SERVER_H

#include <QDialog>
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInterface>

#include "board.h"

class MainWindow;
#include "mainwindow.h"

namespace Ui {
class Server;
}
class Server : public QDialog
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    ~Server();

private:
    Ui::Server *ui;
    QString IP;

public slots:
    void OKButtonSlot();
    void CancelButtonSlot();
};

#endif // SERVER_H

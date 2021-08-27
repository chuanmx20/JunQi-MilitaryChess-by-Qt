#include "mainwindow.h"
#include "board.h"
#include "chess.h"
#include "countdown.h"

#include <QApplication>
#include <QNetworkProxyFactory>

pos Board::positions[60];
int Board::mineLeft = 3;
bool Chess::start = false;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    Board::init();
    MainWindow w;
    w.show();
    return a.exec();
}

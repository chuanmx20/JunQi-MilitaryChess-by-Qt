#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QTcpServer>
#include <QTcpSocket>
#include <QLineEdit>
#include <QTextBrowser>
#include <QByteArray>
#include <QDebug>
#include <QTime>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <algorithm>
#include <cstdlib>
#include <QPainter>
#include <unordered_set>
#include <stack>
#include <QMenuBar>
#include <QLCDNumber>

#include "chess.h"
#include "mainwindow.h"
#include "countdown.h"

namespace Ui {
class Board;
}

struct pos{
    int x;
    int y;
    int row;
    int collum;
    std::string id;
    bool iniPos = true;
    bool onRail;
    std::vector<pos*> neighbors;
};

Chess::TYPE distributeType(int id);
const int xPos[] = {201, 335, 465, 596, 728};
const int yPos[] = {15, 79, 142, 206, 270, 334, 534, 598, 661, 726, 790, 852};

class Board : public QWidget
{
    Q_OBJECT

public:
    explicit Board(QWidget *parent = nullptr, bool isServer = true, QString _ip = "");
    ~Board();
    enum SIDE{Red, Blue, No};
    static struct pos positions[60];
    static void init();
    static struct pos* getPos(int row, int collum);
    bool getRowCol(QPoint point, int& row, int& col);
    Chess* getChess(int row, int col);
    Chess* getChess(int id);
    SIDE pre3Check();
    bool canReach(int row, int col);
    static int mineLeft;
    void kill(Chess* killer, Chess* target);
    void gameOver(bool win);

private:
    Ui::Board *ui;
    const std::string bgImgPath = "./chessboard.png";
    QString IP;
    bool isServer;
    bool mReady = false;
    bool uReady = false;
    bool start = false;
    bool connected = false;
    bool myTurn;
    SIDE side = No;
    std::string pre3 = "";
    int chessInHand = -1;
    Chess* chesses[50];
    void reOrder(QString chessOrder);
    void moveChess(Chess* chess, int row, int col);

    void PaintBoard();
    void PaintBorder(int row, int col);
    void clearBorder();
    bool DFS(struct pos* src, struct pos* dst);
    bool DFSstraight(struct pos* src, struct pos* dst, bool vertical);

    QTcpServer* server;
    QTcpSocket* socket;
    QTextBrowser* textBrowser;
    QPushButton* sendButton;
    QLineEdit* sendEdit;
    QPushButton* ready;
    CountDown* counter;
    QPixmap* pixmap;
    int runOutTime = 0;
    QLabel* timeTip;
    QLabel* turnTip;

    void Start();
    int turnCount = 0;
    QLCDNumber* turnCounter;
    QLabel* myColor;


public slots:
    void Back2Main();
    void NewConnectionSlot();
    void ProcessDataSlot();
    void ChatSlot();
    void ReadyButtonSlot();
    void timeout();
    void quit();
    void shuffleSlot();
    void surrenderSlot();

protected:
    void mouseReleaseEvent(QMouseEvent*);
    void paintEvent(QPaintEvent*);
};

//void init();

#endif // BOARD_H

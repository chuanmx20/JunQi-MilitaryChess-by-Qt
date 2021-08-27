#include "board.h"
#include "ui_board.h"
#include "chess.h"

Board::Board(QWidget *parent, bool _isServer, QString _ip) :
    QWidget(parent),
    ui(new Ui::Board),
    IP(_ip),
    isServer(_isServer)
{
    //window setups
    ui->setupUi(this);
    this->setMinimumSize(1061, 924);
    this->setMaximumSize(1061, 924);

    //dialog browser settings
    textBrowser = new QTextBrowser(this);
    textBrowser->resize(180, 180);
    textBrowser->move(861, 0);
    sendButton = new QPushButton(this);
    sendButton->setText("Send");
    sendButton->resize(180, 50);
    sendButton->move(861, 230);
    sendEdit = new QLineEdit(this);
    sendEdit->resize(180, 50);
    sendEdit->move(861, 180);
    connect(sendButton, SIGNAL(clicked()), this, SLOT(ChatSlot()));
    connect(sendEdit, SIGNAL(returnPressed()), this, SLOT(ChatSlot()));

    //background img set
    pixmap = new QPixmap(":/new/prefix1/empty.jpg");
    ui->board->setPixmap(*pixmap);
    ui->board->move(190, 0);

    //timeout label set
    timeTip = new QLabel("You have run out of time for 0 times", this);
    timeTip->setFixedSize(150, 60);
    timeTip->move(20, 100);
    timeTip->setWordWrap(true);
    timeTip->setAlignment(Qt::AlignTop);
    timeTip->show();

    //turn counter
    QLabel* tcter = new QLabel("Turn:", this);
    tcter->move(861, 600);
    turnCounter = new QLCDNumber(this);
    turnCounter->resize(180, 90);
    turnCounter->move(861, 620);
    turnCounter->display(turnCount);
    turnCounter->show();

    //color tip
    QLabel* colorTip = new QLabel("your color is", this);
    colorTip->setFixedSize(90, 50);
    colorTip->move(861, 400);
    myColor = new QLabel(this);
    myColor->resize(180, 50);
    myColor->move(861, 450);
    myColor->setAutoFillBackground(true);
    myColor->show();



    //turn label set
    turnTip = new QLabel("Its your turn now", this);
    turnTip->setFixedSize(150, 60);
    turnTip->move(20, 160);
    turnTip->setWordWrap(true);
    turnTip->setAlignment(Qt::AlignTop);
    turnTip->hide();

    //creat chesses
    for (int i = 0; i < 50; ++i)
    {
        chesses[i] = new Chess(this, distributeType(i), i);
        chesses[i]->kill();
    }


    //menubar set
    QMenuBar* menubar = new QMenuBar(this);
    QMenu* help = new QMenu("Help", menubar);
    QAction* action1 = new QAction("Leave Room", help);
    QAction* action2 = new QAction("Leave Game", help);
    help->addAction(action1);
    help->addAction(action2);
    menubar->addMenu(help);
    connect(action1, &QAction::triggered, this, &Board::Back2Main);
    connect(action2, &QAction::triggered, this, &Board::quit);

    QMenu* game = new QMenu("Game", menubar);
    QAction* action3 = new QAction("Shuffle Board");
    QAction* action4 = new QAction("Surrender");
    game->addAction(action3);
    game->addAction(action4);
    menubar->addMenu(game);
    connect(action3, &QAction::triggered, this, &Board::shuffleSlot);
    connect(action4, &QAction::triggered, this, &Board::surrenderSlot);


    //ready button set
    ready = new QPushButton(this);
    ready->setText("Ready");
    connect(ready, SIGNAL(clicked()), this, SLOT(ReadyButtonSlot()));
    ready->resize(150, 90);
    ready->move(20, 472);

    //timer set
    counter = new CountDown(this, 20);
    counter->move(20, 382);
    connect(counter, SIGNAL(timeout()), this, SLOT(timeout()));

    //game mode set
    if (isServer)
    {
        /*server mode*/
        this->setWindowTitle("JunQi : Server Mode");
        textBrowser->append("the ip for server is " + IP);
        server = new QTcpServer(this);
        server->listen(QHostAddress::Any, 8081);
        connect(server, SIGNAL(newConnection()), this, SLOT(NewConnectionSlot()));

        /*turn distribution*/
        myTurn = (QRandomGenerator::global()->bounded(0, 100) % 2 == 0);

        /*chess board painting*/
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(chesses, chesses + 50, std::default_random_engine(seed));

    } else {
        /*client mode*/
        this->setWindowTitle("JunQi : Client Mode");
        textBrowser->append("now connecting to " + IP);
        socket = new QTcpSocket(this);
        socket->connectToHost(IP, 8081);
        connect(socket, SIGNAL(readyRead()), this, SLOT(ProcessDataSlot()));
        connected = socket->waitForConnected();
    }

    textBrowser->append("Wait for connection");
}

Board::~Board()
{
    delete ui;
}

void Board::Back2Main()
{
    if (socket)
    {
        char data[1];
        data[0] = '8';
        socket->write(data, 1);
        socket->flush();
    }

    this->close();
    MainWindow* mainwin = new MainWindow();
    mainwin->show();
}

void Board::NewConnectionSlot()
{
    if (socket)
        return;
    socket = server->nextPendingConnection();
    textBrowser->append("Connected");
    //slot get called when new messages sent
    connect(socket, SIGNAL(readyRead()), this, SLOT(ProcessDataSlot()));
    char buf[52];
    buf[0] = '0';
    buf[1] = myTurn ? '1' : '0';    //server begin with buf[1] == 1
    for (int i = 0; i < 50; ++i)
    {
        buf[i + 2]  = chesses[i]->getID();
    }
    socket->write(buf, 52);
    socket->flush();
    connected = true;

}

void Board::reOrder(QString chessOrder)
{
    /*get right order*/
    int order[50];
    for (int i = 0; i < 50; ++i)
    {
        order[i] = chessOrder[i].unicode() - 0;
    }
    /*reOrder the positions array*/
    Chess* newOrder[50];
    for (int i = 0; i < 50; ++i)
    {
        newOrder[i] = getChess(order[i]);
    }
    for (int i = 0; i < 50; ++i)
    {
        chesses[i] = newOrder[i];
    }

}

void Board::ProcessDataSlot()
{
    QByteArray buff = socket->readAll();
    QChar sign = buff[0];
    if (sign.digitValue() == 0)
    {
        /*connection and board data*/
         textBrowser->append("Connected");
         QChar turnSign = buff[1];
         myTurn = (turnSign.digitValue() == 1) ? false : true;      //if server begin, it's not client's turn
         if (myTurn)
             turnTip->show();
         QString chessOrder = buff;
         chessOrder.remove(0, 2);
         reOrder(chessOrder);
    }
    else if (sign.digitValue() == 1)
    {
        /*chat data*/
        QString content = buff;
        //remove the sign character
        content.remove(0, 1);
        textBrowser->append(content);
    }
    else if (sign.digitValue() == 2)
    {
        /*ready data*/
        uReady = !uReady;
        if (mReady && uReady)
            Start();
        else
            start = false;
        QString ready = buff;
        ready.remove(0, 1);
        textBrowser->append(ready);
    }
    else if (sign.digitValue() == 3)
    {
        /*flip data*/
        turnCounter->display(++turnCount);
        counter->reset();
        counter->begin();
        turnTip->show();
        myTurn = true;      //receiving opposite movement signal signs it's my turn now
        getChess((int)buff[1])->flip();
        if (side == No && buff[2] != -1)
        {
            side = buff[2] == 0 ? Blue : Red;

            QPalette palette(side == Red ? Qt::red : Qt::blue);
            myColor->setPalette(palette);
        }
    }
    else if (sign.digitValue() == 4)
    {
        /*shuffle data*/
        textBrowser->append("Board shuffled");
        QString chessOrder = buff;
        chessOrder.remove(0, 1);
        reOrder(chessOrder);
    }
    else if (sign.digitValue() == 5)
    {
        /*movement data*/
        //id + row + col
        turnCounter->display(++turnCount);
        counter->reset();
        counter->begin();
        Chess* tar = getChess((int)buff[1]);
        int row = (int)buff[2];
        int col = (int)buff[3];
        moveChess(tar, row, col);
        turnTip->show();
        myTurn = true;
    }
    else if (sign.digitValue() == 6)
    {
        /*killing happens*/
        //id + id
        turnCounter->display(++turnCount);
        Chess* killer = getChess((int)buff[1]);
        Chess* target = getChess((int)buff[2]);
        if (killer->getType() == Chess::TYPE::Bomb || killer->getType() == target->getType() || target->getType() == Chess::TYPE::Bomb)
            kill(killer, killer);   //suicide
        counter->reset();
        counter->begin();
        kill(killer, target);
        turnTip->show();
        myTurn = true;
    } else if (sign.digitValue() == 7)
    {
        //win or lose
        gameOver((int)buff[1] == 0);
    }
    else if (sign.digitValue() == 8)
    {
        //surrender data
        gameOver(true);
        socket = nullptr;
        connected = false;
        textBrowser->append("Your opponent has left the room");
    }
    else
        qDebug() << "Wrong signal collected";
}

void Board::ChatSlot()
{
    QString content = sendEdit->text();
    sendEdit->clear();
    if (!content.isEmpty())
    {
        content = (isServer ? "Server : " : "Client : ") + content;
        textBrowser->append(content);

        content = '1' + content;
        if (socket)
        {
            socket->write(content.toUtf8().data());
            socket->flush();
        }
    }


}
void Board::Start()
{
    start = true;
    Chess::start = true;
    for (auto i : chesses)
        i->heal();
    PaintBoard();
    counter->begin();
    ready->hide();
    if (myTurn)
        turnTip->show();
}

void Board::ReadyButtonSlot()
{
    if (!connected)
    {
        textBrowser->append("Not connected");
        return;
    }
    if (start)
        return;
    mReady = !mReady;
    ready->setText(mReady ? "Hold On" : "Ready");
    QString content = (isServer) ? "Server : " : "Client : ";
    content += mReady ? "I am ready!" : "I am not ready!";
    textBrowser->append(content);
    content = '2' + content;

    socket->write(content.toUtf8().data());
    socket->flush();
    socket->waitForBytesWritten();
    if (mReady && uReady)
        Start();
    else
        start = false;
}
void Board::timeout()
{
    counter->reset();
    counter->begin();
    if (myTurn)
    {
        turnTip->hide();
        QString tip("You have run out of time for ");
        tip.append(QString::number(++runOutTime));
        tip.append(" times!");
        timeTip->setText(tip);
        if (runOutTime >= 3)
        {
            counter->reset();
            gameOver(false);
            char buf[2];
            buf[0] = '7';
            buf[1] = 0;
            socket->write(buf, 2);
            socket->flush();
        }
    }
    else
    {
        turnTip->show();
    }
    myTurn = !myTurn;
    turnCounter->display(++turnCount);
}


void Board::init()
{
    for (int i = 0; i < 12; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            int index = 5 * i + j;
            Board::positions[index].y = yPos[i];
            Board::positions[index].x = xPos[j];
            Board::positions[index].row = i;
            Board::positions[index].collum = j;
            Board::positions[index].id = std::to_string(index);
            if (index < 10)
                Board::positions[index].id = "0" + Board::positions[index].id;

            if ((j == 1 || j == 3) && (i == 2 || i == 4 || i == 7 || i == 9))
                Board::positions[index].iniPos = false;
            else if ( (j == 2) && (i == 3 || i == 8))
                Board::positions[index].iniPos = false;
            else
                Board::positions[index].iniPos = true;

            if (i > 1 && i < 10 && (j == 0 || j == 4))
                Board::positions[index].onRail = true;
            else if (i == 5 || i == 6 || i == 1 || i == 10)
                Board::positions[index].onRail = true;
            else
                Board::positions[index].onRail = false;
        }
    }

    for (int i = 0; i < 60; ++i)
    {
        if (i == 26 || i == 28 || i == 31 || i == 33)
            continue;
        if (positions[i].row != 0)
            positions[i].neighbors.push_back(getPos(positions[i].row-1, positions[i].collum));
        if (positions[i].collum != 0)
            positions[i].neighbors.push_back(getPos(positions[i].row, positions[i].collum - 1));
        if (positions[i].row != 11)
            positions[i].neighbors.push_back(getPos(positions[i].row + 1, positions[i].collum));
        if (positions[i].collum != 5)
            positions[i].neighbors.push_back(getPos(positions[i].row, positions[i].collum + 1));

        if (!positions[i].iniPos)
        {
            if (i == 17 || i == 42)
                continue;
            positions[i].neighbors.push_back(getPos(positions[i].row - 1, positions[i].collum - 1));
            getPos(positions[i].row - 1, positions[i].collum - 1)->neighbors.push_back(&positions[i]);
            positions[i].neighbors.push_back(getPos(positions[i].row - 1, positions[i].collum + 1));
            getPos(positions[i].row - 1, positions[i].collum + 1)->neighbors.push_back(&positions[i]);
            positions[i].neighbors.push_back(getPos(positions[i].row + 1, positions[i].collum - 1));
            getPos(positions[i].row + 1, positions[i].collum - 1)->neighbors.push_back(&positions[i]);
            positions[i].neighbors.push_back(getPos(positions[i].row + 1, positions[i].collum + 1));
            getPos(positions[i].row + 1, positions[i].collum + 1)->neighbors.push_back(&positions[i]);
        }
    }
    positions[26].neighbors.push_back(&positions[25]);
    positions[26].neighbors.push_back(&positions[27]);
    positions[26].neighbors.push_back(&positions[21]);
    positions[28].neighbors.push_back(&positions[27]);
    positions[28].neighbors.push_back(&positions[29]);
    positions[28].neighbors.push_back(&positions[23]);
    positions[31].neighbors.push_back(&positions[30]);
    positions[31].neighbors.push_back(&positions[32]);
    positions[31].neighbors.push_back(&positions[36]);
    positions[33].neighbors.push_back(&positions[32]);
    positions[33].neighbors.push_back(&positions[34]);
    positions[33].neighbors.push_back(&positions[38]);
}

void Board::PaintBoard()
{
    delete pixmap;
    pixmap = new QPixmap(":/new/prefix1/Board.jpg");
    ui->board->setPixmap(*pixmap);
    int k = 0;
    for (int i = 0; i < 50; ++i)
    {
        if (!positions[k].iniPos)
        {
            ++k;
        }
        chesses[i]->move(positions[k].x, positions[k].y);
        chesses[i]->setRowCol(positions[k].row, positions[k].collum);
        chesses[i]->onRail = positions[k].onRail;
        chesses[i]->heal();
        ++k;
    }

}

Chess::TYPE distributeType(int id)
{
    if (id == 0 || id == 1 || id == 25 || id == 26)
        return Chess::TYPE::Bomb;
    else if (id == 2 || id == 27)
        return Chess::TYPE::Boss;
    else if (id == 3 || id == 28)
        return Chess::TYPE::Jun;
    else if (id == 4 || id == 5 || id == 29 || id == 30)
        return Chess::TYPE::Shi;
    else if (id == 6 || id == 7 || id == 31 || id == 32)
        return Chess::TYPE::Lv;
    else if (id == 8 || id == 9 || id == 33 || id == 34)
        return Chess::TYPE::Tuan;
    else if (id == 10 || id == 11 || id == 35 || id == 36)
        return Chess::TYPE::Ying;
    else if (id == 12 || id == 13 || id == 14 || id == 37 || id == 38 || id == 39)
        return Chess::TYPE::Lian;
    else if (id == 15 || id == 16 || id == 17 || id == 40 || id == 41 || id == 42)
        return Chess::TYPE::Pai;
    else if (id == 18 || id == 19 || id == 20 || id == 43 || id == 44 || id == 45)
        return Chess::TYPE::Gong;
    else if (id == 21 || id == 22 || id == 23 || id == 46 || id == 47 || id == 48)
        return Chess::TYPE::Landmine;
    else
        return Chess::TYPE::Flag;
}

pos* Board::getPos(int row, int collum)
{
    if (row > 11 || row < 0 || collum > 4 || collum < 0)
        return nullptr;
    return &positions[5 * row + collum];
}

void Board::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->pos().x() > 190 && event->pos().x() < 841 && event->pos().y() > 0 && event->pos().y() < 924)
    {
        if (!myTurn || !start)
            return;
        if (event->button() != Qt::LeftButton)
        {
            QWidget::mouseReleaseEvent(event);
            return;
        }
        QPoint point = event->pos();
        int row, col;
        if (getRowCol(point, row, col))
        {
            Chess* pt = getChess(row, col);
            if (chessInHand == -1)  //not selected yet
            {
                if (!pt)
                    return;
                if (!pt->flipped())
                {
                    pt->flip();
                    turnCounter->display(++turnCount);
                    char flipData[3];
                    flipData[0] = '3';
                    flipData[1] = pt->getID();
                    counter->reset();
                    counter->begin();
                    if (side == No)
                    {
                        pre3.append(pt->getID() < 25 ? "0" : "1");
                        if (pre3Check() != No)
                        {
                            side = pre3Check();
                            flipData[2] = side == Red ? 0 : 1;
                            QPalette palette(side == Red ? Qt::red : Qt::blue);
                            myColor->setPalette(palette);
                        }
                    }
                    turnTip->hide();
                    myTurn = false;

                    if (side == No)
                        flipData[2] = -1;
                    else if (side == Red)
                        flipData[2] = 0;
                    else
                        flipData[2] = 1;

                    socket->write(flipData, 3);
                    socket->flush();
                }
                else
                {
                    if (side == No)
                        return;
                    else
                    {
                        SIDE color = pt->getID() < 25 ? Red : Blue;
                        if (side != color)
                            return;
                        //draw border
                        PaintBorder(pt->getRow(), pt->getCol());
                        chessInHand = pt->getID();
                    }
                }
            } else {
                /*now we have selected a chess*/
                Chess* tar = getChess(chessInHand);
                if (!pt)
                {
                    //check if is movable
                    if (!tar->canMove())
                        return;
                    //check if can move to
                    if (!canReach(row, col))
                        return;
                    moveChess(tar, row, col);
                    clearBorder();
                    char moveData[4];
                    moveData[0] = '5';
                    moveData[1] = chessInHand;
                    moveData[2] = row;
                    moveData[3] = col;
                    counter->reset();
                    counter->begin();
                    socket->write(moveData, 4);
                    socket->flush();
                    turnTip->hide();
                    myTurn = false;
                    turnCounter->display(++turnCount);
                    chessInHand = -1;
                } else {
                    if (pt->flipped())
                    {
                        if (pt->getID() == chessInHand)
                            return;
                        SIDE color = pt->getID() < 25 ? Red : Blue;
                        if (side == color)
                        {
                            chessInHand = pt->getID();
                            PaintBorder(pt->getRow(), pt->getCol());
                            ui->board->update();
                        } else {
                            //judge can kill or not
                            if (tar->canKill(pt) && canReach(pt->getRow(), pt->getCol()))
                            {
                                if (pt->getType() == Chess::TYPE::Flag)
                                {
                                    gameOver(true);
                                    char buf[2];
                                    buf[0] = '7';
                                    buf[1] = 1;
                                    socket->write(buf, 2);
                                    socket->flush();
                                    return;
                                }
                                if (tar->getType() == Chess::TYPE::Bomb || tar->getType() == pt->getType() || pt->getType() == Chess::TYPE::Bomb)
                                {
                                    //suicide
                                    kill(tar, tar);
                                }
                                kill(tar, pt);
                                clearBorder();
                                counter->reset();
                                counter->begin();
                                char killData[3];
                                killData[0] = '6';
                                killData[1] = chessInHand;
                                killData[2] = pt->getID();
                                socket->write(killData, 3);
                                socket->flush();
                                turnTip->hide();
                                myTurn = false;
                                turnCounter->display(++turnCount);
                                chessInHand = -1;
                            }
                            else
                                return;
                        }
                    } else {
                        chessInHand = -1;
                        pt->flip();
                        clearBorder();
                        counter->reset();
                        counter->begin();
                        char flipData[2];
                        flipData[0] = '3';
                        flipData[1] = pt->getID();
                        socket->write(flipData, 2);
                        socket->flush();
                        turnTip->hide();
                        myTurn = false;
                        turnCounter->display(++turnCount);
                    }
                }
            }
        }
    } else
        QWidget::mouseReleaseEvent(event);
}

bool Board::getRowCol(QPoint point, int &row, int &col)
{
    for (int i = 0; i < 12; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            QPoint p(xPos[j], yPos[i]);
            int dx = p.x() - point.x();
            int dy = p.y() - point.y();
            if (dx > -100 && dx < 100 && dy > -50 && dy < 50)
            {
                row = i;
                col = j;
                return true;
            }
        }
    }
    return false;
}

Chess* Board::getChess(int row, int col)
{
    Chess* ret = nullptr;
    for (int i = 0; i < 50; ++i)
    {
        if (chesses[i]->getRow() == row && chesses[i]->getCol() == col && !chesses[i]->dead())
        {
            ret = chesses[i];
            break;
        }
    }
    return ret;
}

Board::SIDE Board::pre3Check()
{
    //red is 0 and blue is 1
    if (pre3 == "00" || pre3 == "010" || pre3 == "100")
        return Red;
    else if (pre3 == "11" || pre3 == "101" || pre3 == "011")
        return Blue;
    else return No;
}

void Board::PaintBorder(int row, int col)
{
    delete pixmap;
    pixmap = new QPixmap(":/new/prefix1/Board.jpg");
    QPainter painter(pixmap);
    painter.setPen(QPen(myTurn ? Qt::green : Qt::red, 5));
    painter.drawRect(xPos[col] - 190, yPos[row], 100, 50);
    ui->board->setPixmap(*pixmap);
}

void Board::clearBorder()
{
    delete pixmap;
    pixmap = new QPixmap(":/new/prefix1/Board.jpg");
    ui->board->setPixmap(*pixmap);
}

void Board::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(190, 0, 651, 924, *pixmap);
    this->update();
}

Chess* Board::getChess(int id)
{
    Chess* ret = nullptr;
    for (auto i : chesses)
    {
        if (i->getID() == id)
        {
            ret = i;
            break;
        }
    }
    return ret;
}

void Board::moveChess(Chess *chess, int row, int col)
{
    if (!chess)
    {
        qDebug() << "Wrong movement" << "nullptr" << row << col;
        return;
    }
    if (row < 0 || col < 0)
    {
        qDebug() << "Wrong movement" << row << " " << col;
        return;
    }
    chess->setRowCol(row, col);
    chess->onRail = getPos(row, col)->onRail;
    chess->move(xPos[col], yPos[row]);
}

bool Board::canReach(int row, int col)
{
    Chess* tar = getChess(chessInHand);
    struct pos* begin = getPos(tar->getRow(), tar->getCol());
    struct pos* end = getPos(row, col);
    if (begin == end)
        return true;
    for (auto i : begin->neighbors)
    {
        if (i == end)
            return true;
    }
    if (!tar->onRail || !getPos(row, col)->onRail)
    {
        return false;
    }
    else
    {
        //while tar is not gong, everything get easier
        if (tar->getType() != Chess::TYPE::Gong)
        {
            //now we know they are not neighbors
            if (tar->getRow() != row && tar->getCol() != col)
                return false;
            bool vertical = tar->getCol() == col;
            return DFSstraight(getPos(tar->getRow(), tar->getCol()), getPos(row, col), vertical);

        } else {
            //use DFS to get the result
            return DFS(getPos(tar->getRow(), tar->getCol()), getPos(row, col));
        }
    }
}
/*
bool Board::DFS(struct pos* src, struct pos* dst)
{
    std::stack<struct pos*> s;
    std::unordered_set<struct pos*> visited;
    s.push(src);
    while (!s.empty())
    {
        struct pos* cur = s.top();
        visited.insert(cur);
        s.pop();
        if (cur == dst)
            return true;
        for (auto i : cur->neighbors)
        {
            if (visited.count(i) || !i->onRail)
                continue;
            if (i == dst)
            {
                return true;
            }
            if (getChess(i->row, i->collum))
                continue;
            s.push(i);
        }

    }
    return false;
}*/

bool Board::DFS(struct pos* src, struct pos* dst)
{
   std::stack<struct pos*> s;
   std::unordered_set<struct pos*> visited;
   s.push(src);

   int count = 0;
   while (!s.empty())
   {
       qDebug() << ++count;
       struct pos* cur = s.top();
       if (cur == nullptr) continue;
       s.pop();
       visited.insert(cur);
       for (auto i : cur->neighbors)
       {
           if (i == dst)
               return true;
           if (visited.find(i) != visited.end())
               continue;
           if (i == nullptr)
               continue;
           if (!i->onRail || getChess(i->row, i->collum) != nullptr)    //segmentation fault
               continue;
           s.push(i);
       }
   }
   return false;
}

void Board::kill(Chess* killer, Chess* target)
{
    if (killer == target)   //suicide
    {
        killer->kill();
        return;
    }
    target->kill();
    moveChess(killer, target->getRow(), target->getCol());
    if (target->getType() == Chess::TYPE::Flag)
        gameOver(true);
    else if (target->getType()  == Chess::Landmine && myTurn)
        mineLeft--;
}

void Board::gameOver(bool win)
{
    start = false;      //time out and game ends
    mReady = false;
    uReady = false;
    chessInHand = -1;
    pre3.clear();
    counter->reset();
    ready->show();
    ready->setText("Ready");
    QLabel* tip = new QLabel(nullptr);
    tip->setText(win ?  "You Win!" : "You Lose!");
    tip->resize(250, 180);
    tip->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setPointSize(25);
    tip->setFont(font);
    QPalette palette;
    palette.setColor(QPalette::WindowText, myTurn ? Qt::blue : Qt::red);
    tip->setPalette(palette);
    tip->show();

    turnTip->hide();
    timeTip->setText("You have run out of time for 0 times");
    myColor = new QLabel(this);
    mineLeft = 3;
    runOutTime = 0;
    side = No;
    for (auto i : chesses)
        i->kill();
    delete pixmap;
    pixmap = new QPixmap(":/new/prefix1/empty.jpg");
    ui->board->setPixmap(*pixmap);
}

bool Board::DFSstraight(struct pos *src, struct pos *dst, bool vertical)
{
    if (src == dst)
        return true;
    int offset = src->row + src->collum > dst->row + dst->collum ? -1 : 1;
    bool isNeighbor = false;
    struct pos* nsrc;
    if (vertical)
    {
        nsrc = getPos(src->row + offset, src->collum);
    }
    else
        nsrc = getPos(src->row, src->collum + offset);
    for (auto i : src->neighbors)
    {
        if (i == dst)
            return true;
        if (i == nullptr)
            continue;
        if (getChess(i->row, i->collum))
            continue;
        if (i == nsrc)
        {
            isNeighbor = true;
        }
    }
    if (isNeighbor)
        return DFSstraight(nsrc, dst, vertical);
    return false;
}

void Board::quit()
{
    if (socket)
    {
        char data[1];
        data[0] = '8';
        socket->write(data, 1);
        socket->flush();
    }
    exit(0);
}

void Board::shuffleSlot()
{
    if (!connected)
    {
        textBrowser->append("Not connected");
        return;
    }
    if (start)
    {
        textBrowser->append("You can't shuffle the board when gaming");
        return;
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(chesses, chesses + 50, std::default_random_engine(seed));
    char shuffleData[51];
    shuffleData[0] = '4';
    for (int i = 0; i < 50; ++i)
    {
        shuffleData[i+1] = chesses[i]->getID();
    }
    socket->write(shuffleData, 51);
    socket->flush();
    textBrowser->append("Board shuffled");
}

void Board::surrenderSlot()
{
    if (!start)
    {
        textBrowser->append("You can't surrender as game didn't start");
        return;
    }
    if (turnCount < 20)
    {
        QLabel* tip = new QLabel("It's too early to surrender!", nullptr);
        QFont font;
        font.setPointSize(30);
        tip->setFont(font);
        QPalette palette;
        palette.setColor(QPalette::WindowText, Qt::red);
        tip->setPalette(palette);
        tip->resize(360, 180);
        tip->setAlignment(Qt::AlignCenter);
        tip->show();
        return;
    }
    gameOver(false);
    char data[2];
    data[0] = '7';
    data[1] = 0;
    socket->write(data, 2);
    socket->flush();
}

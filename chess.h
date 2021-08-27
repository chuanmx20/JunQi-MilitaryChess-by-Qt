#ifndef CHESS_H
#define CHESS_H

#include <QObject>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>

class Chess : public QLabel
{
    Q_OBJECT
public:
    enum TYPE{Bomb = 0, Boss = 1, Jun = 2, Shi = 3, Lv = 4, Tuan = 5, Ying = 6, Lian = 7, Pai = 8, Gong = 9, Landmine = 10, Flag = 11};
    const QString unflip = ":/new/prefix1/Unflipped.png";
    explicit Chess(QWidget *parent, Chess::TYPE type, int ID);
    const int width = 150;
    const int height = 100;
    int getID();
    TYPE getType();
    QString GetImgPath();
    void setRowCol(int row, int col);
    int getRow() {return row;}
    int getCol() {return col;}
    static bool start;
    static int compare(Chess l, Chess r);
    /*this function will make comparation between the two chesses
    return 0 if they are the same,
    return 1 if l is bigger then r
    return -1 on opposite situation*/
    bool flipped() {return isFlipped;}
    bool dead() {return isDead;}
    bool onRail;
    bool canMove();
    bool canKill(Chess* tar);
    void kill();
    void heal();

signals:
    void selected(int id);

public slots:
    void flip();
    void clicked();

private:
    TYPE type;
    bool isFlipped;
    bool isDead = false;
    int ID;
    QString imgPath;
    int row, col;
};

#endif // CHESS_H

#include "chess.h"
#include "board.h"

Chess::Chess(QWidget *parent, Chess::TYPE _type, int _ID) :
    QLabel(parent),
    type(_type),
    ID(_ID)
{
    //when obj instantiates, it's not set to safety cell, and is unflipped && alive
    isFlipped = false;
    imgPath = GetImgPath();

    this->setFixedSize(100, 50);
    this->setPixmap(unflip);
    this->setScaledContents(true);

}

void Chess::clicked()
{
    if (!start)
        return;
    if (!isFlipped)
    {
        flip();
        return;
    }

    emit selected(ID);
}

void Chess::flip()
{
    isFlipped = true;
    this->setPixmap(imgPath);
    this->setScaledContents(true);
}

int Chess::getID()
{
    return ID;
}

Chess::TYPE Chess::getType()
{
    return type;
}

QString getName(Chess::TYPE type)
{
    switch (type) {
    case 0:
        return "Bomb";
    case 1:
        return "Boss";
    case 2:
        return "Jun";
    case 3:
        return "Shi";
    case 4:
        return "Lv";
    case 5:
        return "Tuan";
    case 6:
        return "Ying";
    case 7:
        return "Lian";
    case 8:
        return "Pai";
    case 9:
        return "Gong";
    case 10:
        return "LandMine";
    case 11:
        return "Flag";
    }
}

QString Chess::GetImgPath()
{
    QString color = (getID() < 25) ? "Red" : "Blue";
    return (":/new/prefix1/" + getName(getType()) + color + ".png");
}

int Chess::compare(Chess l, Chess r)
{
    if (l.type == r.type)
        return 0;
    if (l.type < r.type)
        return 1;
    else
        return -1;
}

void Chess::setRowCol(int _row, int _col)
{
    row = _row;
    col = _col;
}

bool Chess::canMove()
{
    if (type == Flag || type == Landmine)
        return false;
    return true;
}

bool Chess::canKill(Chess *tar)
{
    if (!Board::positions[tar->row * 5 + tar->col].iniPos && type != Landmine)
        return false;
    //we assume that this is movable
    //suicide is an exception
    if (tar->type == Bomb)
        return true;
    if (tar->type == Landmine)
        return type == Gong || type == Bomb;
    if (tar->type == Flag)
        return (Board::mineLeft == 0) && (type == Gong || type == Bomb);

    return type <= tar->type;
}

void Chess::kill()
{
    isDead = true;
    hide();
}

void Chess::heal()
{
    setPixmap(unflip);
    isFlipped = false;
    isDead = false;
    show();
}

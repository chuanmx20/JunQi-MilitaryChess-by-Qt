#ifndef NUMKEYBOARD_H
#define NUMKEYBOARD_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

class NumKeyboard : public QWidget
{
    Q_OBJECT
public:
    explicit NumKeyboard(QWidget *parent = nullptr);
    std::string getInput();

signals:

private:
    std::string input;

};

#endif // NUMKEYBOARD_H

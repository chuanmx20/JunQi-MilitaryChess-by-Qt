#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QtDebug>
#include <QRegularExpression>

#include "numkeyboard.h"

class MainWindow;
#include "mainwindow.h"

class Client : public QWidget
{
    Q_OBJECT
public:
    explicit Client(QWidget *parent = nullptr);
    bool IPCheck(QString input);

private:
    NumKeyboard* keyboard;

public slots:
    void ConfirmButtonSlot();
    void CancelButtonSlot();

};

#endif // CLIENT_H

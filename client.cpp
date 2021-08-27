#include "client.h"

Client::Client(QWidget *parent) : QWidget(parent)
{
    this->setMaximumSize(360, 320);
    this->setMinimumSize(360, 320);

    QVBoxLayout* layout = new QVBoxLayout(this);
    keyboard = new NumKeyboard();
    layout->addWidget(keyboard);

    QPushButton* confirmButton = new QPushButton("Confirm");
    QPushButton* cancelButton = new QPushButton("Cancel");
    connect(confirmButton, SIGNAL(clicked()), this, SLOT(ConfirmButtonSlot()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(CancelButtonSlot()));

    QHBoxLayout* btnLayout = new QHBoxLayout(this);
    btnLayout->addWidget(confirmButton);
    btnLayout->addWidget(cancelButton);

    layout->addLayout(btnLayout);
    this->setLayout(layout);


}

void Client::CancelButtonSlot()
{
    this->close();
    MainWindow* mainwin = new MainWindow();
    mainwin->show();
}

void Client::ConfirmButtonSlot()
{
    //connect to server
    QString IP = QString::fromStdString(keyboard->getInput());
    if (IPCheck(IP))
    {
        this->close();
        Board* board = new Board(nullptr, false, IP);
        //Board* board = new Board(nullptr, false, "183.172.128.157");
        board->show();
    } else {
        QLabel* tip = new QLabel(nullptr);
        tip->resize(180, 120);
        tip->setAlignment(Qt::AlignCenter);
        tip->setText("Wrong IP Address");
        tip->show();
    }
    return;
}

bool Client::IPCheck(QString input)
{
    if (input.isEmpty())
        return false;
    QRegularExpression rx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    QRegularExpressionMatch match = rx.match(input);

    return match.hasMatch();
}

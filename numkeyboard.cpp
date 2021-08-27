#include "numkeyboard.h"

NumKeyboard::NumKeyboard(QWidget *parent) : QWidget(parent)
{
    //grid layout
    QGridLayout* layout = new QGridLayout();

    //label displayer
    QLabel* displayer = new QLabel();
    displayer->setAlignment(Qt::AlignCenter);
    displayer->setStyleSheet("border: 1px solid green; color: green; background: silver;");
    layout->addWidget(displayer, 0, 0, 1, 3);

    //10 number buttons
    QPushButton* numbers[10];
    numbers[0] = new QPushButton("0");
    layout->addWidget(numbers[0], 4, 1);
    connect(numbers[0], &QPushButton::clicked, this, [=](){input.append("0"); displayer->setText(QString::fromStdString(input));});
    for (int i = 1; i < 10; ++i)
    {
        numbers[i] = new QPushButton(QString::fromStdString(std::to_string(i)));
        layout->addWidget(numbers[i], (i - 1)/3 + 1, (i + 2) % 3);
        connect(numbers[i], &QPushButton::clicked, this, [=](){input.append(std::to_string(i)); displayer->setText(QString::fromStdString(input));});
    }

    //1 dot button
    QPushButton* dotButton = new QPushButton(".");
    layout->addWidget(dotButton, 4, 0);
    connect(dotButton, &QPushButton::clicked, this, [=](){input.append("."); displayer->setText(QString::fromStdString(input));});

    //1 delete button
    QPushButton* delButton = new QPushButton("Delete");
    layout->addWidget(delButton, 4, 2);
    connect(delButton, &QPushButton::clicked, this, [=](){input.pop_back(); displayer->setText(QString::fromStdString(input));});

    //1 clear button
    QPushButton* clearButton = new QPushButton("Clear");
    layout->addWidget(clearButton, 5, 0, 1, 3);
    connect(clearButton, &QPushButton::clicked, this, [=](){input.clear(); displayer->setText(QString::fromStdString(input));});

    this->setLayout(layout);
}

std::string NumKeyboard::getInput()
{
    return input;
}



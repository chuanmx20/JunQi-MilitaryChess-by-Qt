#include "countdown.h"
#include "ui_countdown.h"

CountDown::CountDown(QWidget *parent, int time) :
    QWidget(parent),
    base(time),
    ui(new Ui::CountDown)
{
    ui->setupUi(this);
    timer = new QTimer(this);

    count = base;
    connect(timer, SIGNAL(timeout()), this, SLOT(run()));


    ui->lcdNumber->display(count);
}

CountDown::~CountDown()
{
    delete ui;
}

void CountDown::run()
{
    if (count > 0)
    {
        count--;
        ui->lcdNumber->display(count);
    }
    if (count == 0)
    {
        emit timeout();
    }
}

void CountDown::reset()
{
    count = base;
    ui->lcdNumber->display(count);
    timer->stop();
    disconnect(timer, SIGNAL(timeout()), this, SLOT(start()));
}
void CountDown::start()
{
    if (count > 0)
        timer->start(1000);
    else
        timer->stop();
}
void CountDown::begin()
{
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(start()), Qt::UniqueConnection);
}

void CountDown::stop()
{
    timer->stop();
    count = base;
    ui->lcdNumber->display(count);
}

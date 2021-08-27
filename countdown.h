#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class CountDown;
}

class CountDown : public QWidget
{
    Q_OBJECT

public:
    explicit CountDown(QWidget *parent = nullptr, int time = 20);
    ~CountDown();

private:
    int count = 20;
    int base;
    Ui::CountDown *ui;
    QTimer* timer;

signals:
    void timeout();

public slots:
    void run();
    void reset();
    void start();
    void begin();
    void stop();
};

#endif // COUNTDOWN_H

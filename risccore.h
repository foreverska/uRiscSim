#ifndef RISCCORE_H
#define RISCCORE_H

#include <thread>
#include <functional>
#include <signal.h>

#include <QObject>
#include <QString>

extern "C" {
    #include "littlerisc.h"
}

typedef enum {
    run,
    pause,
    stop,
    done = -1
} threadStatus;

class RiscCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 pc READ pc WRITE setPc NOTIFY pcChanged)
    Q_PROPERTY(QString rpString READ rpString WRITE writeRpS NOTIFY rpsChanged)
    Q_PROPERTY(QString serialString READ serialString
               WRITE writeSerialString NOTIFY serialStringChanged)
    Q_PROPERTY(quint8 gpo READ gpo WRITE writeGpo NOTIFY gpoChanged)

private:
    riscvCore *mpCore;
    std::thread mCoreThread;
    volatile sig_atomic_t mThreadStatus;
    quint32 mPrevPc;
    QString mRpString;
    QString mSerialString;
    quint8 mGpo;

    void RunFunc();
    void StopThread();
    void RunCore();
    void EmulateSerial();
    void EmulateGpioOutput();

protected:

public:
     void StartThread();

    explicit RiscCore(QObject *parent = nullptr);
    ~RiscCore();

quint32 pc() const;
QString rpString() const;
QString serialString() const;
quint8 gpo() const;

signals:
void pcChanged(quint32 pc);
void rpsChanged(QString rpString);
void serialStringChanged(QString serialString);
void gpoChanged(quint8 gpo);

public slots:
    void loadRom(QString filename);
    void runPauseCore();
    void stopCore();
    void resetCore();
    void gpiChanged(quint8 gpi, quint8 value);
    void setPc(quint32 pc);
    void writeRpS(QString rpString);
    void writeSerialString(QString serialString);
    void writeGpo(quint8 gpo);
};

#endif // RISCCORE_H

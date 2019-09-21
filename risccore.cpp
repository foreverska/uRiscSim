#include <QFile>
#include <QUrl>

#include "risccore.h"

#define SERIAL_ADDR 0x1000
#define GPO_ADDR    0x1001
#define GPI_ADDR    0x1002

void RiscCore::loadRom(QString filename)
{
    QUrl url(filename);
    QFile rom(url.toLocalFile());
    char *pRomBytes;
    uint32_t romSize;

    destroyMemory(mpCore);

    if (rom.open(QIODevice::ReadOnly) == false)
    {
        qCritical("Failed to open ROM File\n");
        return;
    }

    romSize = static_cast<uint32_t>(rom.size());
    initMemory(mpCore, romSize);
    pRomBytes = reinterpret_cast<char*>(malloc(romSize));
    if (pRomBytes == nullptr)
    {
        qCritical("Failed to create buffer for ROM load\n");
        return;
    }

    rom.read(pRomBytes, rom.size());
    loadMemory(mpCore, reinterpret_cast<uint8_t*>(pRomBytes), 0, romSize);

    qDebug("Rom Loaded Successfully");
}

quint32 RiscCore::pc() const
{
    return mpCore->pc;
}

QString RiscCore::rpString() const
{
    return mRpString;
}

QString RiscCore::serialString() const
{
    return mSerialString;
}

quint8 RiscCore::gpo() const
{
    return mGpo;
}

void RiscCore::setPc(quint32 pc)
{
    if (mPrevPc == pc)
        return;

    mPrevPc = pc;
    mpCore->pc = pc;
    emit pcChanged(mPrevPc);
}

void RiscCore::writeRpS(QString rpString)
{
    if (mRpString == rpString)
        return;

    mRpString = rpString;
    emit rpsChanged(mRpString);
}

void RiscCore::writeSerialString(QString serialString)
{
    if (mSerialString == serialString)
        return;

    mSerialString = serialString;
    emit serialStringChanged(mSerialString);
}

void RiscCore::writeGpo(quint8 gpo)
{
    if (mGpo == gpo)
        return;

    mGpo = gpo;
    emit gpoChanged(mGpo);
}

void RiscCore::EmulateSerial()
{
    if (mpCore->mmu.pMemory[SERIAL_ADDR] != 0)
    {
        QString temp = mSerialString +
                static_cast<char>(mpCore->mmu.pMemory[SERIAL_ADDR]);
        writeSerialString(temp);
        mpCore->mmu.pMemory[SERIAL_ADDR] = 0;
    }
}

void RiscCore::EmulateGpioOutput()
{
    writeGpo(mpCore->mmu.pMemory[GPO_ADDR]);
}

void RiscCore::RunCore()
{
    coreExecute(mpCore);
    EmulateSerial();
    EmulateGpioOutput();
}

void RiscCore::RunFunc()
{
    sig_atomic_t tempStatus = mThreadStatus;
    sig_atomic_t prevStatus = mThreadStatus;
    while (mThreadStatus != threadStatus::done)
    {
        tempStatus = mThreadStatus;
        if (prevStatus != tempStatus)
        {
            switch (tempStatus)
            {
            case threadStatus::stop:
                writeRpS("Play");
                writeSerialString("");
                break;
            default:
                break;
            }
        }

        switch (tempStatus)
        {
        case threadStatus::run:
            RunCore();
            break;
        case threadStatus::pause:
            setPc(mpCore->pc);
            break;
        case threadStatus::stop:
            setPc(0);
            break;
        case threadStatus::done:
            break;
        default:
            qFatal("Thread Status unknown value\n");
        }

        prevStatus = tempStatus;
    }
}

void RiscCore::StartThread()
{
    mThreadStatus = threadStatus::stop;
    std::function<void(void)> threadFunc = std::bind(&RiscCore::RunFunc, this);
    mCoreThread = std::thread(threadFunc);
}

void RiscCore::StopThread()
{
    mThreadStatus = threadStatus::done;
    while (mCoreThread.joinable() != true) {}
    mCoreThread.join();
}

void RiscCore::runPauseCore()
{
    if (mThreadStatus == run)
    {
        writeRpS("Play");
        mThreadStatus = threadStatus::pause;
    }
    else
    {
        writeRpS("Pause");
        mThreadStatus = threadStatus::run;
    }
}

void RiscCore::stopCore()
{
    mThreadStatus = threadStatus::stop;
}

void RiscCore::resetCore()
{

}

void RiscCore::gpiChanged(quint8 gpi, quint8 value)
{
    uint8_t gpiValue = mpCore->mmu.pMemory[GPI_ADDR];
    if (value != 0)
    {
        gpiValue |= 1<<gpi;
    }
    else
    {
        gpiValue &= (0xFF ^ (1<<gpi));
    }
    mpCore->mmu.pMemory[GPI_ADDR] = gpiValue;
}

RiscCore::RiscCore(QObject *parent) : QObject(parent)
{
    initCore(&mpCore);

    mPrevPc = 0;
    writeRpS("Play");
    writeSerialString("");
    writeGpo(0);
    mThreadStatus = threadStatus::stop;
}

RiscCore::~RiscCore()
{
    StopThread();
    destroyMemory(mpCore);
    destroyCore(mpCore);
}

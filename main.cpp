#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <risccore.h>

int main(int argc, char *argv[])
{
    RiscCore riscvCore;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    riscvCore.StartThread();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("riscvCore", &riscvCore);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

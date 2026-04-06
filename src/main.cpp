#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "controller/SimulationController.h"
#include "models/EventLogModel.h"
#include "models/MemoryTableModel.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("MemSim");
    app.setApplicationVersion("1.0");

    qmlRegisterType<MemoryTableModel>("MemSim", 1, 0, "MemoryTableModel");
    qmlRegisterType<EventLogModel>("MemSim", 1, 0, "EventLogModel");

    SimulationController simController;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("simController", &simController);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "decompileproject.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    DecompiledObject::qml_register();
    DecompileProject::qml_register();
    QGuiApplication app(argc, argv);

    app.setOrganizationName("uglyoldbob");
    app.setOrganizationDomain("uglyoldbob.com");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

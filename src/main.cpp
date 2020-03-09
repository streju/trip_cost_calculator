#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "AppProperties.hpp"
#include "SqlConnectionHelper.hpp"
#include "WarningIndicator.hpp"

void registerModels(QQmlApplicationEngine &engine, AppProperties &appProperties)
{
    for (const auto& property : appProperties.properties_)
    {
        engine.rootContext()->setContextProperty(property.first, property.second);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    const auto sqlHelper = std::make_shared<SqlConnectionHelper>();
    AppProperties properties;
    registerModels(engine, properties);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

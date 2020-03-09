#include "SqlConnectionHelper.hpp"

#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlError>

SqlConnectionHelper::SqlConnectionHelper()
{
    connect();
}

SqlConnectionHelper::~SqlConnectionHelper()
{
    if (isDatabaseOpen())
    {
        disconnect();
        db_.close();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
}

bool SqlConnectionHelper::connect()
{
    if(!isDatabaseOpen())
    {
        if(!db_.isValid())
        {
            db_ = QSqlDatabase::addDatabase("QSQLITE");
            if (!db_.isValid())
            {
                qFatal("Cannot add database: %s", qPrintable(db_.lastError().text()));
            }
        }
        const QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (!writeDir.mkpath("."))
        {
            qFatal("Failed to create writable directory at %s", qPrintable(writeDir.absolutePath()));
        }
        const QString databaseName = writeDir.absolutePath() + "/database.sqlite3";
        db_.setDatabaseName(databaseName);
        if (!isDatabaseOpen())
        {
            qFatal("Cannot open database: %s", qPrintable(db_.lastError().text()));
            QFile::remove(databaseName);
        }
    }
    return isDatabaseOpen();
}

bool SqlConnectionHelper::isDatabaseOpen()
{
    return db_.open();
}

void SqlConnectionHelper::disconnect()
{
    qDebug() << "Disconnecting database";
    db_.close();
}

QSqlDatabase& SqlConnectionHelper::getDatabase()
{
    return db_;
}

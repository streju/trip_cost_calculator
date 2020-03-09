#ifndef SQLSERVICE_HPP
#define SQLSERVICE_HPP

#include <QtSql>

class SqlConnectionHelper
{
public:
    SqlConnectionHelper(bool isTest = false);
    ~SqlConnectionHelper();
    QSqlDatabase& getDatabase();
    bool isDatabaseOpen();

private:
    bool connect();
    void disconnect();

    QSqlDatabase db_;
    QString dbName_;
};

#endif // SQLSERVICE_HPP

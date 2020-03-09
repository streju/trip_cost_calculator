#ifndef SQLSERVICE_HPP
#define SQLSERVICE_HPP

#include <QtSql>

class SqlConnectionHelper
{
public:
    SqlConnectionHelper();
    ~SqlConnectionHelper();
    QSqlDatabase& getDatabase();
    bool isDatabaseOpen();

private:
    bool connect();
    void disconnect();

    QSqlDatabase db_;

};

#endif // SQLSERVICE_HPP

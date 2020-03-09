#ifndef QSQLQUERYMOCK_HPP
#define QSQLQUERYMOCK_HPP

#include <gmock/gmock.h>
#include <QString>
#include "Interfaces/IQSqlQuery.hpp"

class QSqlQueryMock : public IQSqlQuery
{
public:
    MOCK_METHOD0(exec, bool());
    MOCK_METHOD1(exec, bool(const QString&));
    MOCK_METHOD0(next, bool());
    MOCK_METHOD1(prepare, bool(const QString&));
    MOCK_METHOD2(bindValue, void(const QString&, const QVariant&));
    MOCK_CONST_METHOD1(value, QVariant(int));
    MOCK_CONST_METHOD1(value, QVariant(const QString&));
    MOCK_CONST_METHOD0(lastError, QSqlError());
    MOCK_METHOD0(clear, void());
    MOCK_METHOD0(get, QSqlQuery());
};

#endif // QSQLQUERYMOCK_HPP


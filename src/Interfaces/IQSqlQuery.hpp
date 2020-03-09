#ifndef IQSQLQUERY_HPP
#define IQSQLQUERY_HPP

#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QVariant>

class IQSqlQuery
{
public:
    virtual bool exec() = 0;
    virtual bool exec(const QString& query) = 0;
    virtual bool next() = 0;
    virtual bool prepare(const QString& query) = 0;
    virtual void bindValue(const QString& placeholder, const QVariant& val) = 0;
    virtual QVariant value(int i) const = 0;
    virtual QVariant value(const QString& name) const = 0;
    virtual QSqlError lastError() const = 0;
    virtual void clear() = 0;
    virtual QSqlQuery get() = 0;
    virtual ~IQSqlQuery() = default;
};

#endif // IQSQLQUERY_HPP

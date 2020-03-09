#ifndef QSQLQUERYWRAPPER_HPP
#define QSQLQUERYWRAPPER_HPP

#include <QSqlQuery>
#include <QString>
#include <QVariant>

#include "Interfaces/IQSqlQuery.hpp"

class QSqlQueryWrapper : public IQSqlQuery
{
public:
    QSqlQueryWrapper() {}
    QSqlQueryWrapper(const QString& queryStr) : query_(queryStr) {}

    bool exec() override {return query_.exec();}
    bool exec(const QString& query) override {return query_.exec(query);}
    bool next() override {return query_.next();}
    bool prepare(const QString& query) override {return query_.prepare(query);}
    void bindValue(const QString& placeholder, const QVariant& val) override {return query_.bindValue(placeholder, val);}
    QVariant value(int i) const override {return query_.value(i);}
    QVariant value(const QString& name) const override {return query_.value(name);}
    QSqlError lastError() const override {return query_.lastError();}
    void clear() override {query_.clear();}
    QSqlQuery get() override {return query_;}
private:
    QSqlQuery query_;
};

#endif // QSQLQUERYWRAPPER_HPP

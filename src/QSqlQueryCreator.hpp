#ifndef QSQLQUERYCREATOR_HPP
#define QSQLQUERYCREATOR_HPP

#include <memory>
#include <QString>

#include "Interfaces/IQSqlQueryCreator.hpp"
#include "QSqlQueryWrapper.hpp"

class QSqlQueryCreator : public IQSqlQueryCreator
{
public:
    QSqlQueryCreator() = default;
    std::shared_ptr<IQSqlQuery> create() override
    {
        return std::make_shared<QSqlQueryWrapper>();
    }
    void create(const QString& querystr) override
    {
        QSqlQueryWrapper{querystr};
    }
};

#endif // QSQLQUERYCREATOR_HPP

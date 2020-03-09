#ifndef IQSQLQUERYCREATOR_HPP
#define IQSQLQUERYCREATOR_HPP

#include <memory>
#include <QString>

#include "Interfaces/IQSqlQuery.hpp"

class IQSqlQueryCreator
{
public:
    virtual std::shared_ptr<IQSqlQuery> create() = 0;
    virtual void create(const QString& query) = 0;
    virtual ~IQSqlQueryCreator() = default;
};

using QSqlQueryCreatorPtr = std::shared_ptr<IQSqlQueryCreator>;

#endif // IQSQLQUERYCREATOR_HPP

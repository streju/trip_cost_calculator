#ifndef QSQLQUERYCREATORMOCK_HPP
#define QSQLQUERYCREATORMOCK_HPP

#include <gmock/gmock.h>
#include <QString>
#include "Interfaces/IQSqlQueryCreator.hpp"

class QSqlQueryCreatorMock : public IQSqlQueryCreator
{
public:
    MOCK_METHOD1(create, void(const QString&));
    MOCK_METHOD0(create, std::shared_ptr<IQSqlQuery>());
};

#endif // QSQLQUERYCREATORMOCK_HPP


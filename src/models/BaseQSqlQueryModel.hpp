#ifndef BASEQSQLQUERYMODEL_HPP
#define BASEQSQLQUERYMODEL_HPP

#include <QObject>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>

class BaseQSqlQueryModel : public QSqlQueryModel
{
public:
    explicit BaseQSqlQueryModel(QObject* parent = nullptr) : QSqlQueryModel(parent)
    {}
    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role < Qt::UserRole)
             return QSqlQueryModel::data(index, role);

         const QSqlRecord sqlRecord = record(index.row());
         QVariant value = sqlRecord.value(role - Qt::UserRole);
         return value;
    }
    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> names;
        for( int i = 0; i < record().count(); i ++)
        {
            names.insert(Qt::UserRole + i, record().fieldName(i).toUtf8());
        }
        return names;
    }

    void refreshModel()
    {
        const auto prevQuery = query().executedQuery();
        clear();
        query().clear();
        setQuery(prevQuery);
    }
};

#endif // BASEQSQLQUERYMODEL_HPP

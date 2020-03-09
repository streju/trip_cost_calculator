#ifndef ACTIVITIESMODEL_HPP
#define ACTIVITIESMODEL_HPP

#include <QString>

#include <QAbstractTableModel>

#include "BaseQSqlQueryModel.hpp"
#include "CurrentStateProvider.hpp"

class ActivitiesModel : public QAbstractTableModel
{
public:
    explicit ActivitiesModel(const CurrentStateProviderPtr& currentStateProvider, QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QHash<int,QByteArray> roleNames() const;
    void setQuery();
private:
    struct Activity{
        Activity(const QString& data, const QString& added_by, const QString& description)
            : data(data), added_by(added_by), description(description)
        {}
        friend bool operator<(const Activity& l, const Activity& r)
        {
            return l.data < r.data; // TODO fix sort
        }
        QString data;
        QString added_by;
        QString description;
    };

    CurrentStateProviderPtr currentStateProvider_;
    QStringList headers_{"data", "added_by", "description"};
    QStringList rows_;
    std::vector<QStringList> data_;
};

class GroupActivitiesModel : public BaseQSqlQueryModel
{
public:
    explicit GroupActivitiesModel(const CurrentStateProviderPtr& currentStateProvider);
    void setQuery();
signals:

private:
    Q_OBJECT

    CurrentStateProviderPtr currentStateProvider_;
};

using GroupActivitiesModelPtr = GroupActivitiesModel* const;

#endif // ACTIVITIESMODEL_HPP

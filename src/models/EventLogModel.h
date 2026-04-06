#pragma once

#include <QAbstractListModel>

#include "core/SimEvent.h"

class EventLogModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        TimestampRole = Qt::UserRole + 1,
        AlgorithmRole,
        TypeRole,
        DetailRole,
    };

    explicit EventLogModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setEvents(const QVector<SimEvent>& events);

private:
    QVector<SimEvent> m_events;
};

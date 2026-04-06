#include "EventLogModel.h"

EventLogModel::EventLogModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int EventLogModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_events.size();
}

QVariant EventLogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto& event = m_events[index.row()];
    switch (role) {
    case TimestampRole:
        return event.timestamp.toString("hh:mm:ss.zzz");
    case AlgorithmRole:
        return event.algorithmName;
    case TypeRole:
        return static_cast<int>(event.type);
    case DetailRole:
        return event.detail;
    default:
        return {};
    }
}

QHash<int, QByteArray> EventLogModel::roleNames() const
{
    return {
        { TimestampRole, "timestamp" },
        { AlgorithmRole, "algorithm" },
        { TypeRole, "type" },
        { DetailRole, "detail" },
    };
}

void EventLogModel::setEvents(const QVector<SimEvent>& events)
{
    beginResetModel();
    m_events = events;
    endResetModel();
}

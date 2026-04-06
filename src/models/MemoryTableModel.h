#pragma once

#include <QAbstractTableModel>

#include "core/MemoryBlock.h"

class MemoryTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit MemoryTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setBlocks(const QVector<MemoryBlock>& blocks);

private:
    QVector<MemoryBlock> m_blocks;
};

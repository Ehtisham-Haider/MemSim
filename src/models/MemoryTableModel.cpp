#include "MemoryTableModel.h"

MemoryTableModel::MemoryTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int MemoryTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_blocks.size();
}

int MemoryTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 5;
}

QVariant MemoryTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }

    const auto& block = m_blocks[index.row()];
    switch (index.column()) {
    case 0:
        return QStringLiteral("BLK-%1").arg(block.blockId);
    case 1:
        return block.startAddress;
    case 2:
        return block.sizeKB;
    case 3:
        return block.statusString();
    case 4:
        return block.isFree ? QStringLiteral("-") : block.processLabel;
    default:
        return {};
    }
}

QVariant MemoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case 0:
        return QStringLiteral("Block ID");
    case 1:
        return QStringLiteral("Start Addr");
    case 2:
        return QStringLiteral("Size (KB)");
    case 3:
        return QStringLiteral("Status");
    case 4:
        return QStringLiteral("Process");
    default:
        return {};
    }
}

void MemoryTableModel::setBlocks(const QVector<MemoryBlock>& blocks)
{
    beginResetModel();
    m_blocks = blocks;
    endResetModel();
}

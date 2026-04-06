#include "ExportManager.h"

#include <QFile>
#include <QTextStream>

namespace {
QString eventTypeName(SimEvent::Type type)
{
    switch (type) {
    case SimEvent::INIT:
        return QStringLiteral("INIT");
    case SimEvent::ALLOC_SUCCESS:
        return QStringLiteral("ALLOC_OK");
    case SimEvent::ALLOC_FAIL:
        return QStringLiteral("ALLOC_FAIL");
    case SimEvent::DEALLOC:
        return QStringLiteral("DEALLOC");
    case SimEvent::SPLIT:
        return QStringLiteral("SPLIT");
    case SimEvent::MERGE:
        return QStringLiteral("MERGE");
    case SimEvent::STRESS:
        return QStringLiteral("STRESS");
    case SimEvent::RESET:
        return QStringLiteral("RESET");
    }
    return QStringLiteral("UNKNOWN");
}
}

bool ExportManager::exportLog(const QString& filePath,
    const QVariantList& allStates,
    const QVector<SimEvent>& events,
    const QVector<FragmentationReport>& reports,
    const QVariantMap& comparison,
    const QVariantList& stressRows,
    QString& error) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        error = QStringLiteral("Failed to open file: %1").arg(file.errorString());
        return false;
    }

    QTextStream stream(&file);
    stream << "MemSim Export Log\n";
    stream << "=================\n\n";

    stream << "Initial / Current State\n";
    stream << "-----------------------\n";
    for (const auto& stateVariant : allStates) {
        const auto map = stateVariant.toMap();
        stream << map.value("algorithm").toString() << "\n";
        const auto blocks = map.value("blocks").toList();
        for (const auto& b : blocks) {
            const auto block = b.toMap();
            stream << "  BLK-" << block.value("blockId").toInt()
                   << " start=" << block.value("startAddress").toInt()
                   << " size=" << block.value("sizeKB").toInt()
                   << " status=" << block.value("status").toString()
                   << " proc=" << block.value("processLabel").toString() << "\n";
        }
        stream << "\n";
    }

    stream << "Event Log\n";
    stream << "---------\n";
    for (const auto& event : events) {
        stream << event.timestamp.toString(Qt::ISODateWithMs) << " | "
               << event.algorithmName << " | "
               << eventTypeName(event.type) << " | "
               << event.detail << "\n";
    }

    stream << "\nFragmentation Reports\n";
    stream << "---------------------\n";
    for (const auto& report : reports) {
        stream << report.algorithmName
               << " free=" << report.totalFreeKB
               << " allocated=" << report.totalAllocatedKB
               << " freeBlocks=" << report.freeBlockCount
               << " largest=" << report.largestFreeBlockKB
               << " ratio=" << report.fragmentationRatio
               << " external=" << (report.externalFragmentationPresent ? "YES" : "NO")
               << " failed=" << report.failedAllocations << "\n";
    }

    stream << "\nComparative Summary\n";
    stream << "-------------------\n";
    for (auto it = comparison.begin(); it != comparison.end(); ++it) {
        stream << it.key() << ": " << it.value().toString() << "\n";
    }

    stream << "\nStress Test Rows\n";
    stream << "----------------\n";
    for (const auto& row : stressRows) {
        const auto map = row.toMap();
        stream << "Req #" << map.value("request").toInt() << " size=" << map.value("sizeKB").toInt()
               << " | FF=" << map.value("First-Fit").toString()
               << " | NF=" << map.value("Next-Fit").toString()
               << " | BF=" << map.value("Best-Fit").toString()
               << " | WF=" << map.value("Worst-Fit").toString() << "\n";
    }

    return true;
}

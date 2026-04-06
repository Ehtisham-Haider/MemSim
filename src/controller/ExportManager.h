#pragma once

#include <QVector>

#include "core/FragmentationReport.h"
#include "core/SimEvent.h"

class ExportManager {
public:
    bool exportLog(const QString& filePath,
        const QVariantList& allStates,
        const QVector<SimEvent>& events,
        const QVector<FragmentationReport>& reports,
        const QVariantMap& comparison,
        const QVariantList& stressRows,
        QString& error) const;
};

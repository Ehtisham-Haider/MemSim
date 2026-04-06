#pragma once

#include <QVariantMap>
#include <QVector>

#include "core/FragmentationReport.h"

class ComparativeAnalyser {
public:
    QVariantMap buildReport(const QVector<FragmentationReport>& reports,
        const QVariantMap& successStats) const;
    QString generateJustification(const QVector<FragmentationReport>& reports,
        const QVariantMap& successStats) const;
};

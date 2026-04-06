#include "ComparativeAnalyser.h"

#include <QStringList>

QVariantMap ComparativeAnalyser::buildReport(const QVector<FragmentationReport>& reports,
    const QVariantMap& successStats) const
{
    QVariantMap allocationSpeed {
        {"firstFit", "Fast"},
        {"nextFit", "Fast"},
        {"bestFit", "Slow"},
        {"worstFit", "Slow"},
    };

    QVariantMap largestFreeBlocks;
    QVariantMap fragmentationRatios;
    QVariantMap externalFragmentation;

    for (const auto& report : reports) {
        const QString key = report.algorithmName;
        largestFreeBlocks.insert(key, report.largestFreeBlockKB);
        fragmentationRatios.insert(key, report.fragmentationRatio);
        externalFragmentation.insert(key, report.externalFragmentationPresent ? "Present" : "Low");
    }

    QVariantMap suitability {
        {"First-Fit", "General-purpose workloads"},
        {"Next-Fit", "Streaming and sequential allocations"},
        {"Best-Fit", "Tight allocations where immediate waste must be low"},
        {"Worst-Fit", "Variable request sizes with larger leftovers"},
    };

    QVariantMap internalFrag {
        {"First-Fit", "N/A"},
        {"Next-Fit", "N/A"},
        {"Best-Fit", "N/A"},
        {"Worst-Fit", "N/A"},
    };

    QVariantMap reportMap;
    reportMap.insert("allocationSpeed", allocationSpeed);
    reportMap.insert("internalFragmentation", internalFrag);
    reportMap.insert("externalFragmentation", externalFragmentation);
    reportMap.insert("suitability", suitability);
    reportMap.insert("successRates", successStats);
    reportMap.insert("largestFreeBlocks", largestFreeBlocks);
    reportMap.insert("fragmentationRatios", fragmentationRatios);
    reportMap.insert("justification", generateJustification(reports, successStats));
    return reportMap;
}

QString ComparativeAnalyser::generateJustification(const QVector<FragmentationReport>& reports,
    const QVariantMap& successStats) const
{
    Q_UNUSED(reports)
    const QString first = successStats.value("First-Fit", 0.0).toString();
    const QString next = successStats.value("Next-Fit", 0.0).toString();
    const QString best = successStats.value("Best-Fit", 0.0).toString();
    const QString worst = successStats.value("Worst-Fit", 0.0).toString();

    QStringList lines;
    lines << "First-Fit: Fastest practical search with simple implementation, but tends to fragment early regions.";
    lines << "Next-Fit: Similar speed while distributing allocations with cursor advancement for better balance.";
    lines << "Best-Fit: Minimizes immediate leftover space, but often creates many tiny unusable fragments.";
    lines << "Worst-Fit: Leaves larger remainders that may improve future fit opportunities in mixed workloads.";
    lines << QStringLiteral("Observed success rates — First-Fit: %1%, Next-Fit: %2%, Best-Fit: %3%, Worst-Fit: %4%.")
                 .arg(first)
                 .arg(next)
                 .arg(best)
                 .arg(worst);
    return lines.join('\n');
}

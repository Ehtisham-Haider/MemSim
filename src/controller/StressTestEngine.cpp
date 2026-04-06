#include "StressTestEngine.h"

#include <QRandomGenerator>

StressTestEngine::StressTestEngine(QObject* parent)
    : QObject(parent)
{
}

void StressTestEngine::run(AlgorithmEngine& first,
    AlgorithmEngine& next,
    AlgorithmEngine& best,
    AlgorithmEngine& worst,
    const QVector<MemoryBlock>& initial,
    bool useFixedSeed,
    quint32 fixedSeed)
{
    first.resetPool(initial);
    next.resetPool(initial);
    best.resetPool(initial);
    worst.resetPool(initial);

    QRandomGenerator seeded(fixedSeed);

    auto randomSize = [&]() {
        if (useFixedSeed) {
            return seeded.bounded(10, 151);
        }
        return QRandomGenerator::global()->bounded(10, 151);
    };

    QVariantList rows;
    int ffSuccess = 0;
    int nfSuccess = 0;
    int bfSuccess = 0;
    int wfSuccess = 0;

    for (int request = 1; request <= 10; ++request) {
        const int sizeKB = randomSize();
        const int processId = 99 + request;
        const QString processLabel = QStringLiteral("S%1").arg(request);

        QVariantMap row;
        row.insert("request", request);
        row.insert("sizeKB", sizeKB);

        const auto ff = first.allocate(sizeKB, processId, processLabel);
        const auto nf = next.allocate(sizeKB, processId, processLabel);
        const auto bf = best.allocate(sizeKB, processId, processLabel);
        const auto wf = worst.allocate(sizeKB, processId, processLabel);

        row.insert("First-Fit", ff.isSuccess() ? QStringLiteral("✓ BLK-%1").arg(ff.blockId) : QStringLiteral("✗ Failed"));
        row.insert("Next-Fit", nf.isSuccess() ? QStringLiteral("✓ BLK-%1").arg(nf.blockId) : QStringLiteral("✗ Failed"));
        row.insert("Best-Fit", bf.isSuccess() ? QStringLiteral("✓ BLK-%1").arg(bf.blockId) : QStringLiteral("✗ Failed"));
        row.insert("Worst-Fit", wf.isSuccess() ? QStringLiteral("✓ BLK-%1").arg(wf.blockId) : QStringLiteral("✗ Failed"));

        ffSuccess += ff.isSuccess() ? 1 : 0;
        nfSuccess += nf.isSuccess() ? 1 : 0;
        bfSuccess += bf.isSuccess() ? 1 : 0;
        wfSuccess += wf.isSuccess() ? 1 : 0;

        rows.push_back(row);

        emit requestCompleted(0, request, ff.isSuccess(), row.value("First-Fit").toString());
        emit requestCompleted(1, request, nf.isSuccess(), row.value("Next-Fit").toString());
        emit requestCompleted(2, request, bf.isSuccess(), row.value("Best-Fit").toString());
        emit requestCompleted(3, request, wf.isSuccess(), row.value("Worst-Fit").toString());
        emit progressUpdated(request, 10);
    }

    QVariantMap summary;
    summary.insert("First-Fit", ffSuccess * 10.0);
    summary.insert("Next-Fit", nfSuccess * 10.0);
    summary.insert("Best-Fit", bfSuccess * 10.0);
    summary.insert("Worst-Fit", wfSuccess * 10.0);

    emit stressTestFinished(rows, summary);
}

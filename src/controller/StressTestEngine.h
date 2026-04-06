#pragma once

#include <QObject>
#include <QVariantList>

#include "algorithms/AlgorithmEngine.h"
#include "core/AllocationRequest.h"

class StressTestEngine : public QObject {
    Q_OBJECT
public:
    explicit StressTestEngine(QObject* parent = nullptr);

    void run(AlgorithmEngine& first,
        AlgorithmEngine& next,
        AlgorithmEngine& best,
        AlgorithmEngine& worst,
        const QVector<MemoryBlock>& initial,
        bool useFixedSeed,
        quint32 fixedSeed);

signals:
    void progressUpdated(int requestIndex, int total);
    void requestCompleted(int algorithmIndex, int requestIndex, bool success, const QString& detail);
    void stressTestFinished(QVariantList results, QVariantMap summary);
};

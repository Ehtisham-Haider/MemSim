#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

#include "ComparativeAnalyser.h"
#include "ExportManager.h"
#include "StressTestEngine.h"
#include "algorithms/AlgorithmEngine.h"

class SimulationController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList allAlgorithmStates READ allAlgorithmStates NOTIFY stateChanged)
    Q_PROPERTY(QVariantList eventLog READ eventLog NOTIFY eventLogChanged)
    Q_PROPERTY(bool simulationRunning READ simulationRunning NOTIFY simulationRunningChanged)
    Q_PROPERTY(QVariantMap comparativeReport READ comparativeReport NOTIFY comparativeReportChanged)
    Q_PROPERTY(QVariantList stressResults READ stressResults NOTIFY stressResultsChanged)
    Q_PROPERTY(QVariantMap stressSummary READ stressSummary NOTIFY stressSummaryChanged)
    Q_PROPERTY(int stressProgress READ stressProgress NOTIFY stressProgressChanged)

public:
    explicit SimulationController(QObject* parent = nullptr);

    Q_INVOKABLE void allocate(int sizeKB, int processId, const QString& label, const QString& algorithmName = QStringLiteral("All"));
    Q_INVOKABLE void deallocate(int blockId, const QString& algorithmName);
    Q_INVOKABLE void resetAll();
    Q_INVOKABLE void loadPreset(const QString& presetName);
    Q_INVOKABLE QVariantList getFragmentationReports() const;
    Q_INVOKABLE void exportLog(const QString& filePath);
    Q_INVOKABLE void runStressTest(bool useFixedSeed = false, int fixedSeed = 0);

    QVariantList allAlgorithmStates() const;
    QVariantList eventLog() const;
    bool simulationRunning() const;
    QVariantMap comparativeReport() const;
    QVariantList stressResults() const;
    QVariantMap stressSummary() const;
    int stressProgress() const;

signals:
    void stateChanged();
    void eventLogChanged();
    void simulationRunningChanged();
    void comparativeReportChanged();
    void stressResultsChanged();
    void stressSummaryChanged();
    void stressProgressChanged();
    void userMessage(const QString& message);

private:
    QVector<MemoryBlock> defaultPool() const;
    void refreshComparativeReport();
    QVariantMap poolState(const AlgorithmEngine& engine) const;
    QVariantList eventsToVariantList(const QVector<SimEvent>& events) const;
    QVector<SimEvent> allEvents() const;
    AlgorithmEngine* engineByName(const QString& algorithmName);
    const AlgorithmEngine* engineByName(const QString& algorithmName) const;

    AlgorithmEngine m_firstFit;
    AlgorithmEngine m_nextFit;
    AlgorithmEngine m_bestFit;
    AlgorithmEngine m_worstFit;

    StressTestEngine m_stress;
    ComparativeAnalyser m_comparativeAnalyser;
    ExportManager m_exportManager;

    bool m_simulationRunning = false;
    QVariantMap m_comparativeReport;
    QVariantList m_stressResults;
    QVariantMap m_stressSummary;
    int m_stressProgress = 0;
};

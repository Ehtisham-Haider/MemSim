#include "SimulationController.h"

#include <algorithm>

SimulationController::SimulationController(QObject* parent)
    : QObject(parent)
    , m_firstFit(AlgorithmType::FirstFit, this)
    , m_nextFit(AlgorithmType::NextFit, this)
    , m_bestFit(AlgorithmType::BestFit, this)
    , m_worstFit(AlgorithmType::WorstFit, this)
    , m_stress(this)
{
    resetAll();

    connect(&m_stress, &StressTestEngine::progressUpdated, this, [this](int index, int total) {
        m_stressProgress = static_cast<int>((index * 100.0) / total);
        emit stressProgressChanged();
    });

    connect(&m_stress, &StressTestEngine::stressTestFinished, this, [this](const QVariantList& rows, const QVariantMap& summary) {
        m_stressResults = rows;
        m_stressSummary = summary;
        m_simulationRunning = false;
        refreshComparativeReport();
        emit stressResultsChanged();
        emit stressSummaryChanged();
        emit simulationRunningChanged();
        emit stateChanged();
        emit eventLogChanged();
    });
}

void SimulationController::allocate(int sizeKB, int processId, const QString& label, const QString& algorithmName)
{
    if (algorithmName == QStringLiteral("All")) {
        m_firstFit.allocate(sizeKB, processId, label);
        m_nextFit.allocate(sizeKB, processId, label);
        m_bestFit.allocate(sizeKB, processId, label);
        m_worstFit.allocate(sizeKB, processId, label);
    } else if (auto* engine = engineByName(algorithmName)) {
        engine->allocate(sizeKB, processId, label);
    }

    refreshComparativeReport();
    emit stateChanged();
    emit eventLogChanged();
}

void SimulationController::deallocate(int blockId, const QString& algorithmName)
{
    if (algorithmName == QStringLiteral("All")) {
        m_firstFit.deallocate(blockId);
        m_nextFit.deallocate(blockId);
        m_bestFit.deallocate(blockId);
        m_worstFit.deallocate(blockId);
    } else if (auto* engine = engineByName(algorithmName)) {
        engine->deallocate(blockId);
    }

    refreshComparativeReport();
    emit stateChanged();
    emit eventLogChanged();
}

void SimulationController::resetAll()
{
    const auto initial = defaultPool();

    m_firstFit.resetPool(initial);
    m_nextFit.resetPool(initial);
    m_bestFit.resetPool(initial);
    m_worstFit.resetPool(initial);

    m_stressResults.clear();
    m_stressSummary.clear();
    m_stressProgress = 0;

    refreshComparativeReport();

    emit stateChanged();
    emit eventLogChanged();
    emit stressResultsChanged();
    emit stressSummaryChanged();
    emit stressProgressChanged();
}

void SimulationController::loadPreset(const QString& presetName)
{
    Q_UNUSED(presetName)
    resetAll();

    allocate(45, 1, QStringLiteral("P1"), QStringLiteral("All"));
    allocate(120, 2, QStringLiteral("P2"), QStringLiteral("All"));
    allocate(30, 3, QStringLiteral("P3"), QStringLiteral("All"));
    allocate(200, 4, QStringLiteral("P4"), QStringLiteral("All"));
    allocate(350, 5, QStringLiteral("P5"), QStringLiteral("All"));

    deallocate(0, QStringLiteral("All"));
    deallocate(2, QStringLiteral("All"));

    allocate(60, 6, QStringLiteral("P6"), QStringLiteral("All"));
}

QVariantList SimulationController::getFragmentationReports() const
{
    QVariantList list;

    const QVector<AlgorithmEngine const*> engines { &m_firstFit, &m_nextFit, &m_bestFit, &m_worstFit };
    for (const auto* engine : engines) {
        const auto report = engine->generateFragmentationReport();
        QVariantMap map;
        map.insert("algorithm", report.algorithmName);
        map.insert("totalFreeKB", report.totalFreeKB);
        map.insert("freeBlockCount", report.freeBlockCount);
        map.insert("largestFreeBlockKB", report.largestFreeBlockKB);
        map.insert("externalFragmentation", report.externalFragmentationPresent);
        map.insert("fragmentationRatio", report.fragmentationRatio);
        map.insert("totalAllocatedKB", report.totalAllocatedKB);
        map.insert("failedAllocations", report.failedAllocations);
        list.push_back(map);
    }

    return list;
}

void SimulationController::exportLog(const QString& filePath)
{
    QString error;
    const auto reportsRaw = QVector<FragmentationReport> {
        m_firstFit.generateFragmentationReport(),
        m_nextFit.generateFragmentationReport(),
        m_bestFit.generateFragmentationReport(),
        m_worstFit.generateFragmentationReport(),
    };

    if (!m_exportManager.exportLog(filePath,
            allAlgorithmStates(),
            allEvents(),
            reportsRaw,
            m_comparativeReport,
            m_stressResults,
            error)) {
        emit userMessage(error);
    } else {
        emit userMessage(QStringLiteral("Export successful: %1").arg(filePath));
    }
}

void SimulationController::runStressTest(bool useFixedSeed, int fixedSeed)
{
    m_simulationRunning = true;
    m_stressProgress = 0;
    emit simulationRunningChanged();
    emit stressProgressChanged();

    m_stress.run(m_firstFit,
        m_nextFit,
        m_bestFit,
        m_worstFit,
        defaultPool(),
        useFixedSeed,
        static_cast<quint32>(fixedSeed));
}

QVariantList SimulationController::allAlgorithmStates() const
{
    QVariantList list;
    list.push_back(poolState(m_firstFit));
    list.push_back(poolState(m_nextFit));
    list.push_back(poolState(m_bestFit));
    list.push_back(poolState(m_worstFit));
    return list;
}

QVariantList SimulationController::eventLog() const
{
    return eventsToVariantList(allEvents());
}

bool SimulationController::simulationRunning() const
{
    return m_simulationRunning;
}

QVariantMap SimulationController::comparativeReport() const
{
    return m_comparativeReport;
}

QVariantList SimulationController::stressResults() const
{
    return m_stressResults;
}

QVariantMap SimulationController::stressSummary() const
{
    return m_stressSummary;
}

int SimulationController::stressProgress() const
{
    return m_stressProgress;
}

QVector<MemoryBlock> SimulationController::defaultPool() const
{
    return {
        MemoryBlock { 0, 0, 60, true, -1, QString() },
        MemoryBlock { 1, 60, 100, true, -1, QString() },
        MemoryBlock { 2, 160, 30, true, -1, QString() },
        MemoryBlock { 3, 190, 80, true, -1, QString() },
        MemoryBlock { 4, 270, 50, true, -1, QString() },
        MemoryBlock { 5, 320, 200, true, -1, QString() },
        MemoryBlock { 6, 520, 40, true, -1, QString() },
        MemoryBlock { 7, 560, 150, true, -1, QString() },
        MemoryBlock { 8, 710, 90, true, -1, QString() },
        MemoryBlock { 9, 800, 224, true, -1, QString() },
    };
}

void SimulationController::refreshComparativeReport()
{
    const QVector<FragmentationReport> reports {
        m_firstFit.generateFragmentationReport(),
        m_nextFit.generateFragmentationReport(),
        m_bestFit.generateFragmentationReport(),
        m_worstFit.generateFragmentationReport(),
    };

    QVariantMap successRates;
    const QVector<const AlgorithmEngine*> engines { &m_firstFit, &m_nextFit, &m_bestFit, &m_worstFit };
    for (const auto* engine : engines) {
        const int attempts = std::max(1, engine->allocationAttempts());
        const double rate = (engine->allocationSuccesses() * 100.0) / attempts;
        successRates.insert(engine->name(), QString::number(rate, 'f', 1));
    }

    m_comparativeReport = m_comparativeAnalyser.buildReport(reports, successRates);
    emit comparativeReportChanged();
}

QVariantMap SimulationController::poolState(const AlgorithmEngine& engine) const
{
    QVariantMap map;
    map.insert("algorithm", engine.name());
    map.insert("shortName", engine.shortName());

    QVariantList blocks;
    const auto& pool = engine.pool();
    for (const auto& block : pool.blocks) {
        QVariantMap b;
        b.insert("blockId", block.blockId);
        b.insert("startAddress", block.startAddress);
        b.insert("sizeKB", block.sizeKB);
        b.insert("isFree", block.isFree);
        b.insert("status", block.statusString());
        b.insert("processId", block.processId);
        b.insert("processLabel", block.processLabel);
        blocks.push_back(b);
    }

    const auto report = engine.generateFragmentationReport();
    map.insert("blocks", blocks);
    map.insert("totalFreeKB", report.totalFreeKB);
    map.insert("totalAllocatedKB", report.totalAllocatedKB);
    map.insert("freeBlockCount", report.freeBlockCount);
    map.insert("largestFreeBlockKB", report.largestFreeBlockKB);
    map.insert("fragmentationRatio", report.fragmentationRatio);
    map.insert("externalFragmentation", report.externalFragmentationPresent);
    map.insert("nextFitCursor", pool.nextFitCursor);
    return map;
}

QVariantList SimulationController::eventsToVariantList(const QVector<SimEvent>& events) const
{
    QVariantList list;
    for (const auto& event : events) {
        QVariantMap row;
        row.insert("timestamp", event.timestamp.toString("hh:mm:ss.zzz"));
        row.insert("algorithm", event.algorithmName);
        row.insert("type", static_cast<int>(event.type));
        row.insert("detail", event.detail);
        row.insert("blockIdA", event.blockIdA);
        row.insert("blockIdB", event.blockIdB);
        row.insert("processId", event.processId);
        list.push_back(row);
    }
    return list;
}

QVector<SimEvent> SimulationController::allEvents() const
{
    QVector<SimEvent> all;
    const QVector<AlgorithmEngine const*> engines { &m_firstFit, &m_nextFit, &m_bestFit, &m_worstFit };
    for (const auto* engine : engines) {
        const auto& events = engine->eventLog();
        all += events;
    }

    std::sort(all.begin(), all.end(), [](const SimEvent& lhs, const SimEvent& rhs) {
        return lhs.timestamp < rhs.timestamp;
    });

    return all;
}

AlgorithmEngine* SimulationController::engineByName(const QString& algorithmName)
{
    if (algorithmName == QStringLiteral("First-Fit")) {
        return &m_firstFit;
    }
    if (algorithmName == QStringLiteral("Next-Fit")) {
        return &m_nextFit;
    }
    if (algorithmName == QStringLiteral("Best-Fit")) {
        return &m_bestFit;
    }
    if (algorithmName == QStringLiteral("Worst-Fit")) {
        return &m_worstFit;
    }
    return nullptr;
}

const AlgorithmEngine* SimulationController::engineByName(const QString& algorithmName) const
{
    return const_cast<SimulationController*>(this)->engineByName(algorithmName);
}

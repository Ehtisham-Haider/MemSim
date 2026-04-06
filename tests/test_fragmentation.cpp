#include <QtTest>

#include "test_common.h"

class FragmentationTest : public QObject {
    Q_OBJECT
private slots:
    void reportContainsExpectedMetrics()
    {
        AlgorithmEngine engine(AlgorithmType::FirstFit);
        engine.resetPool(defaultTestPool());
        engine.allocate(45, 1, "P1");

        const auto report = engine.generateFragmentationReport();
        QCOMPARE(report.algorithmName, QString("First-Fit"));
        QVERIFY(report.totalFreeKB > 0);
        QVERIFY(report.freeBlockCount > 0);
        QVERIFY(report.largestFreeBlockKB > 0);
    }
};

QTEST_APPLESS_MAIN(FragmentationTest)
#include "test_fragmentation.moc"

#include <QtTest>

#include "test_common.h"

class WorstFitTest : public QObject {
    Q_OBJECT
private slots:
    void worstFitPicksLargest()
    {
        AlgorithmEngine engine(AlgorithmType::WorstFit);
        engine.resetPool(defaultTestPool());

        auto result = engine.allocate(30, 1, "P1");
        QVERIFY(result.isSuccess());
        QCOMPARE(result.blockId, 9);
    }
};

QTEST_APPLESS_MAIN(WorstFitTest)
#include "test_worstfit.moc"

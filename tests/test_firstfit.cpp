#include <QtTest>

#include "test_common.h"

class FirstFitTest : public QObject {
    Q_OBJECT
private slots:
    void firstFitSelectsEarliestSuitableBlock()
    {
        AlgorithmEngine engine(AlgorithmType::FirstFit);
        engine.resetPool(defaultTestPool());

        auto result = engine.allocate(45, 1, "P1");
        QVERIFY(result.isSuccess());
        QCOMPARE(result.blockId, 0);
    }
};

QTEST_APPLESS_MAIN(FirstFitTest)
#include "test_firstfit.moc"

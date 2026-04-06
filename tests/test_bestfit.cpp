#include <QtTest>

#include "test_common.h"

class BestFitTest : public QObject {
    Q_OBJECT
private slots:
    void bestFitPicksSmallestValid()
    {
        AlgorithmEngine engine(AlgorithmType::BestFit);
        engine.resetPool(defaultTestPool());

        auto result = engine.allocate(28, 1, "P1");
        QVERIFY(result.isSuccess());
        QCOMPARE(result.blockId, 2);
    }
};

QTEST_APPLESS_MAIN(BestFitTest)
#include "test_bestfit.moc"

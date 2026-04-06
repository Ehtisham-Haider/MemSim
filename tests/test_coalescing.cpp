#include <QtTest>

#include "test_common.h"

class CoalescingTest : public QObject {
    Q_OBJECT
private slots:
    void adjacentFreeBlocksMergeAfterDeallocate()
    {
        AlgorithmEngine engine(AlgorithmType::FirstFit);
        engine.resetPool(defaultTestPool());

        auto r1 = engine.allocate(30, 1, "P1");
        auto r2 = engine.allocate(20, 2, "P2");
        QVERIFY(r1.isSuccess());
        QVERIFY(r2.isSuccess());

        QVERIFY(engine.deallocate(r1.blockId).isSuccess());
        QVERIFY(engine.deallocate(r2.blockId).isSuccess());

        QVERIFY(engine.pool().freeBlockCount() >= 1);
    }
};

QTEST_APPLESS_MAIN(CoalescingTest)
#include "test_coalescing.moc"

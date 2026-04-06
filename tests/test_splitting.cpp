#include <QtTest>

#include "test_common.h"

class SplittingTest : public QObject {
    Q_OBJECT
private slots:
    void splitOccursWhenRemainderAboveTwenty()
    {
        AlgorithmEngine engine(AlgorithmType::FirstFit);
        engine.resetPool(defaultTestPool());

        auto result = engine.allocate(10, 1, "P1");
        QVERIFY(result.isSuccess());
        QVERIFY(result.newBlockId >= 0);
        QVERIFY(result.splitRemainder > 20);
    }
};

QTEST_APPLESS_MAIN(SplittingTest)
#include "test_splitting.moc"

#include <QtTest>

#include "test_common.h"

class NextFitTest : public QObject {
    Q_OBJECT
private slots:
    void nextFitWrapsAround()
    {
        AlgorithmEngine engine(AlgorithmType::NextFit);
        engine.resetPool(defaultTestPool());

        QVERIFY(engine.allocate(200, 1, "P1").isSuccess());
        QVERIFY(engine.allocate(150, 2, "P2").isSuccess());
        auto result = engine.allocate(40, 3, "P3");

        QVERIFY(result.isSuccess());
    }
};

QTEST_APPLESS_MAIN(NextFitTest)
#include "test_nextfit.moc"

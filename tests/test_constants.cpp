#include <QtTest/QTest>
#include "constants.h"

class TestConstants : public QObject {
    Q_OBJECT

private slots:
    void appVersion_matchesProjectVersion()
    {
        QCOMPARE(QString(AppConstants::APP_VERSION), QString(EXPECTED_VERSION));
    }
};

QTEST_MAIN(TestConstants)
#include "test_constants.moc"

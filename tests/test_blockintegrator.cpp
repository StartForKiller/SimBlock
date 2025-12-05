#include <QtTest/QtTest>

#include <items/blocks/blockintegrator.hpp>
#include <items/blocks/blockderivator.hpp>
#include <solver/testsolver.hpp>

#include "utils.hpp"

using namespace Tests;

class BlockIntegratorTest : public QObject {
    Q_OBJECT

    private slots:
        void test_integrator() {
            Blocks::BlockIntegrator integrator;
            Solver::TestSolver testSolver(&integrator);

            testSolver.setInput(Solver::make_signal(1.0));

            double t = 0.0;
            const double dt = 0.01;

            for(int i = 0; i < 100; i++) {
                testSolver.step(dt);
                t += dt;
            }

            QCOMPARE(std::get<double>(testSolver.output().data), 1.0 * t);
        }

        void test_derivative() {
            Blocks::BlockDerivator derivator;
            Solver::TestSolver testSolver(&derivator);

            testSolver.setInput(Solver::make_signal(1.0));

            testSolver.step(0.000);
            QCOMPARE(std::get<double>(testSolver.output().data), 100.0);

            testSolver.step(0.001);
            QCOMPARE(std::get<double>(testSolver.output().data), 90.0);

            testSolver.step(0.001);
            QCOMPARE(std::get<double>(testSolver.output().data), 81.0);

            testSolver.step(0.001);
            QCOMPARE(std::get<double>(testSolver.output().data), 72.9);
        }
};

QTEST_MAIN(BlockIntegratorTest)
#include "test_blockintegrator.moc"
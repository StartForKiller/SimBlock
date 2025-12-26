#pragma once

#include <QVector>

namespace Solver {
    struct Signal;
};

namespace Scope {

struct ScopeSample {
    double x;
    double y;
};

enum class ChannelKind {
    Scalar,
    VectorElement,
    BusField,
};

struct ChannelLayout {
    QString name;
    ChannelKind kind;
    int index;
};

class ScopeChannel {
    public:
        QString name;
        QString unit;

        void push(double x, double y);
        const QVector<ScopeSample>& data() const;

    private:
        QVector<ScopeSample> _buffer;
};

class SignalAdapter {
    public:
        static QVector<ChannelLayout> explode(
            const Solver::Signal &signal,
            const QString &baseName
        );
};

}
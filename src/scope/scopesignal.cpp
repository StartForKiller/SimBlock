#include <scope/scopesignal.hpp>
#include <solver/solver.hpp>

using namespace Scope;

void ScopeChannel::push(double x, double y) {
    _buffer.push_back({x, y});
}

const QVector<ScopeSample>& ScopeChannel::data() const {
    return _buffer;
}

QVector<ChannelLayout> SignalAdapter::explode(
    const Solver::Signal &signal,
    const QString &baseName
) {
    QVector<ChannelLayout> out;

    if(isScalar(signal)) {
        out.push_back({
            baseName,
            ChannelKind::Scalar,
            0
        });
    } else if(isVector(signal)) {
        auto vec = std::get<QVector<double>>(signal.data);
        for(int i = 0; i < vec.size(); i++) {
            out.push_back({
                baseName + "[" + QString::number(i) + "]",
                ChannelKind::VectorElement,
                i
            });
        }
    } else if(isBus(signal)) {
        auto bus = std::get<Solver::Signal::Bus>(signal.data);

        int i = 0;
        for(const auto &field : bus.asKeyValueRange()) {
            out.push_back({
                field.first,
                ChannelKind::BusField,
                i++
            });
        }
    }

    return out;
}

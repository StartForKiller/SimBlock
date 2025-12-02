#include <items/blocks/blocktf.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/itemtypes.hpp>

#include <qschematic/scene.hpp>
#include <qschematic/items/label.hpp>
#include <qschematic/commands/item_remove.hpp>
#include <qschematic/commands/item_visibility.hpp>
#include <qschematic/commands/label_rename.hpp>

#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>
#include <QGraphicsDropShadowEffect>
#include <QJsonDocument>
#include <QJsonArray>

using namespace Blocks;

BlockTF::BlockTF(QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockTFType, parent)
{
    setSize(40, 40);
    setBaseName(QStringLiteral("Transfer Function"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { true, 0, QPoint(0, 1), QStringLiteral("in") },
        { false, 0, QPoint(2, 1), QStringLiteral("out") }
    };
    setupConnectors(connectorAttributes);

    _numeratorCoefs.resize(1);
    _denominatorCoefs.resize(1);
    _numeratorCoefs[0] = 1;
    _denominatorCoefs[0] = 1;
    updateMatrix();

    setDescription(QStringLiteral("Applies a transfer function to the input"));
    addProperty({
        "Numerators", Properties::BLOCK_PROPERTY_STRING,
        0, 0,
        [&](const QVariant &v) {
            auto stringValue = v.toString();
            QJsonDocument doc = QJsonDocument::fromJson(stringValue.toUtf8());
            if(!doc.isArray()) return;

            QJsonArray arr = doc.array();
            for (const QJsonValue &v : arr)
                if(!v.isDouble()) return;

            _numeratorCoefs.resize(arr.size());
            for(int i = 0; i < arr.size(); i++)
                _numeratorCoefs[i] = arr[i].toDouble();

            updateMatrix();
        },
        [&]() {
            QJsonArray arr;

            for(auto v : _numeratorCoefs)
                arr.append(v);

            QJsonDocument doc(arr);
            return QVariant(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)).replace(",", ", "));
        }
    });
    addProperty({
        "Denominators", Properties::BLOCK_PROPERTY_STRING,
        0, 0,
        [&](const QVariant &v) {
            auto stringValue = v.toString();
            QJsonDocument doc = QJsonDocument::fromJson(stringValue.toUtf8());
            if(!doc.isArray()) return;

            QJsonArray arr = doc.array();
            for (const QJsonValue &v : arr)
                if(!v.isDouble()) return;

            _denominatorCoefs.resize(arr.size());
            for(int i = 0; i < arr.size(); i++)
                _denominatorCoefs[i] = arr[i].toDouble();

            updateMatrix();
        },
        [&]() {
            QJsonArray arr;

            for(auto v : _denominatorCoefs)
                arr.append(v);

            QJsonDocument doc(arr);
            return QVariant(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)).replace(",", ", "));
        }
    });

    //setConnectorsMovable(false);
}

gpds::container BlockTF::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());

    for(int i = 0; i < _numeratorCoefs.size(); i++) {
        gpds::container numerator;
        numerator.add_attribute("index", i);
        numerator.add_value("value", _numeratorCoefs[i]);
        root.add_value("numerator", numerator);
    }
    for(int i = 0; i < _denominatorCoefs.size(); i++) {
        gpds::container denominator;
        denominator.add_attribute("index", i);
        denominator.add_value("value", _denominatorCoefs[i]);
        root.add_value("denominator", denominator);
    }

    return root;
}

void BlockTF::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());

    auto numerators = container.get_values<gpds::container*>("numerator");
    auto denominators = container.get_values<gpds::container*>("denominator");
    std::sort(numerators.begin(), numerators.end(), [](gpds::container* a, gpds::container* b) {
        std::optional<int> index1 = a->get_attribute<int>("index");
        std::optional<int> index2 = b->get_attribute<int>("index");
        if (!index1.has_value() || !index2.has_value()) {
            qCritical("Wire::from_container(): Numerator has no index.");
            return false;
        }
        return index1.value() < index2.value();
    });
    std::sort(denominators.begin(), denominators.end(), [](gpds::container* a, gpds::container* b) {
        std::optional<int> index1 = a->get_attribute<int>("index");
        std::optional<int> index2 = b->get_attribute<int>("index");
        if (!index1.has_value() || !index2.has_value()) {
            qCritical("Wire::from_container(): Denominator has no index.");
            return false;
        }
        return index1.value() < index2.value();
    });

    _numeratorCoefs.clear();
    _denominatorCoefs.clear();
    for(const gpds::container* numeratorContainer : numerators) {
        _numeratorCoefs.append(numeratorContainer->get_value<double>("value").value_or(0));
    }
    for(const gpds::container* denominatorContainer : denominators) {
        _denominatorCoefs.append(denominatorContainer->get_value<double>("value").value_or(0));
    }

    updateMatrix();
}

std::shared_ptr<QSchematic::Items::Item> BlockTF::deepCopy() const {
    auto clone = std::make_shared<BlockTF>(parentItem());
    copyAttributes(*clone);

    return clone;
}

void BlockTF::copyAttributes(BlockTF &dest) const {
    BaseBlock::copyAttributes(dest);

    dest._numeratorCoefs = _numeratorCoefs;
    dest._denominatorCoefs = _denominatorCoefs;
}

Solver::BlockType BlockTF::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("tf %1").arg((int)_A.size()), //tf (states)
        1, //Inputs
        1, //Outputs
        (int)_A.size(), //States

        (_D == 0.0) //Only true for blocks that doesn't cause algebraic loops
    };
}

void BlockTF::updateMatrix() {
    if(_denominatorCoefs.isEmpty() || _numeratorCoefs.isEmpty()) {
        //TODO: Error, imposible
        return;
    }

    QVector<double> num = _numeratorCoefs;
    QVector<double> den = _denominatorCoefs;

    while (num.size() > 1 && std::fabs(num[0]) < 1e-14) num.removeFirst();
    while (den.size() > 1 && std::fabs(den[0]) < 1e-14) den.removeFirst();

    int degNum = num.size() - 1;
    int degDen = den.size() - 1;

    if(degDen == 0) {
        double den0 = den[0];

        _A.clear(); _B.clear(); _C.clear();

        if(degNum == 0) {
            _D = num[0] / den0;
        } else {
            //Mayor numerador que denominador, imposible
            _D = num.last() / den0;
        }
        return;
    }

    double a0 = den[0];
    for(double &d : den) d /= a0;
    for(double &b : num) b /= a0;

    QVector<double> R = num;
    double D = 0.0;

    while(R.size() >= den.size()) {
        double ratio = R[0];
        D += ratio;

        for(int i = 0; i < den.size(); i++) {
            R[i] -= ratio * den[i];
        }

        while(!R.isEmpty() && std::fabs(R[0]) < 1e-14) {
            R.removeFirst();
        }
    }

    int n = degDen;

    while(R.size() < n)
        R.prepend(0.0);

    _A.clear(); _B.clear(); _C.clear();
    _A.resize(n, QVector<double>(n, 0.0));
    _B.resize(n, 0.0);
    _C.resize(n, 0.0);
    _D = D;

    for(int i = 0; i < n - 1; i++) {
        _A[i][i + 1] = 1.0;
    }

    for(int j = 1; j <= n; j++) {
        _A[n - 1][n - j] = -den[j];
    }

    _B[n - 1] = 1.0;

    std::reverse(R.begin(), R.end());
    for(int i = 0; i < n; i++) {
        _C[i] = R[i];
    }
}

void BlockTF::solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) {
    Solver::Signal outValue = Solver::make_signal(0.0);
    for(int i = 0; i < _C.size(); i++)
        outValue += _C[i] * states[i];
    out[0] = outValue + _D * in[0];
}

void BlockTF::solveDerivative(const QVector<Solver::Signal> &in, const QVector<Solver::Signal> &states, QVector<Solver::Signal> &dx) {
    for(int i = 0; i < _A.size(); i++) {
        Solver::Signal temp = Solver::make_signal(0.0);
        for(int j = 0; j < _A.size(); j++) {
            temp += _A[i][j] * states[j];
        }
        dx[i] = temp + _B[i] * in[0];
    }
}

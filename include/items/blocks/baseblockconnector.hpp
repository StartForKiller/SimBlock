#pragma once

#include <qschematic/items/connector.hpp>

namespace Blocks {

class BaseBlockConnector : public QSchematic::Items::Connector {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BaseBlockConnector)

    public:
        BaseBlockConnector(const QPoint& gridPos = QPoint(), const QString& text = QString(), bool input = false, uint index = 0, QGraphicsItem *parent = nullptr);
        ~BaseBlockConnector() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;
        std::unique_ptr<QWidget> popup() const override;
        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
        void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

        bool isInput() const {
            return _isInput;
        }
        int index() const {
            return _index;
        }

    protected:
        bool _isInput;
        uint _index;

        void copyAttributes(BaseBlockConnector &dest) const;
};

}
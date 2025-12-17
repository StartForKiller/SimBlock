#pragma once

#include <qschematic/view.hpp>

namespace Windows {

class View : public QSchematic::View {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(View)

    public:
        explicit View(QWidget* parent = nullptr);
        ~View() override = default;

    protected:
        // View overrides
        void keyPressEvent(QKeyEvent* event) override;
};

}
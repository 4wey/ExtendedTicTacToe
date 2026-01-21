#pragma once

#include <QWidget>
#include <QString>

class QPushButton;
class QGridLayout;
class QResizeEvent;

class BoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit BoardWidget(QWidget* parent = nullptr);
    ~BoardWidget() override;

    void setBoardSize(int n);
    void resetBoard();

    void setCellText(int r, int c, const QString& text, bool enabled);

signals:
    void cellClicked(int r, int c);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void rebuild();
    void updateCellVisuals();

private:
    int N_ = 3;
    QGridLayout* grid_ = nullptr;

    QPushButton** cells_ = nullptr;
    int cellsCount_ = 0;
};

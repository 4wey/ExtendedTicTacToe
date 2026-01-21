#pragma once

#include <QWidget>
#include <QString>
#include <QVector>

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

    // Score mode: веса клеток (отображаются только при включенном флаге)
    void setCellWeight(int r, int c, int weight);
    void setShowWeights(bool on);
    bool showWeights() const { return showWeights_; }

    // Фиксация размера клетки (в пикселях). Если 0 — автоматическая подгонка.
    void setCellSizePx(int px);

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

    int* weights_ = nullptr;
    bool showWeights_ = false;
    int cellSizePx_ = 0;
};

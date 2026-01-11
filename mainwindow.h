#pragma once

#include <QMainWindow>

#include "game/game_engine.h"

class BoardWidget;
class QComboBox;
class QCheckBox;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onModeChanged(int index);
    void onFillModeChanged(int index);
    void onShowWeightsToggled(bool on);
    void onNewGameClicked();
    void onRulesClicked();
    void onCellClicked(int r, int c);

private:
    void setupUi();
    void applyModeUi();          // включ/выкл контролы, размер клеток
    void applyFillUi();          // применить fillMode в engine
    void refreshBoardAll();      // обновить X/O, enabled, веса
    void refreshStatus();        // подписи, очки, активная строка/столбец
    void finishIfNeeded(const MoveOutcome& out);

    // Иконки генерируем программно, чтобы не зависеть от файлов/ресурсов
    static QIcon makeXIcon(int px);
    static QIcon makeOIcon(int px);

private:
    GameEngine engine_;

    BoardWidget* board_ = nullptr;

    QComboBox* modeCombo_ = nullptr;
    QComboBox* fillCombo_ = nullptr;
    QCheckBox* showWeightsBox_ = nullptr;

    QPushButton* newGameBtn_ = nullptr;
    QPushButton* rulesBtn_ = nullptr;

    QLabel* statusLabel_ = nullptr;
    QLabel* scoreLabel_ = nullptr;
};

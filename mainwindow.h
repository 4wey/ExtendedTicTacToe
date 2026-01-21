#pragma once

#include <QMainWindow>

#include "game/game_engine.h"

class BoardWidget;
class QPushButton;
class QComboBox;
class QLabel;
class QCheckBox;

enum class UiLang { RU, EN };

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onNewGame();
    void onShowStats();
    void onShowRules();

    void onLanguageChanged(int index);
    void onModeChanged(int index);
    void onFillModeChanged(int index);
    void onPlayerTypeChanged();

    void onShowWeightsToggled(bool checked);

    void onCellClicked(int r, int c);

private:
    void buildUi();
    void retranslateUi();

    void syncUiToEngine();
    void refreshBoard();
    void updateInfoLabel();
    void updateRulesButton();
    void updateShowWeightsControls();

    QString stripeInfoText() const;

    void maybeScheduleComputer();
    void doComputerStep();
    void applyFinishedResult(const MoveOutcome& out);

private:
    GameEngine engine_;

    BoardWidget* board_ = nullptr;

    QPushButton* btnNewGame_ = nullptr;
    QPushButton* btnStats_ = nullptr;
    QPushButton* btnRules_ = nullptr;

    QLabel* lblLanguage_ = nullptr;
    QComboBox* cbLanguage_ = nullptr;

    QLabel* lblMode_ = nullptr;
    QComboBox* cbMode_ = nullptr;

    QLabel* lblFill_ = nullptr;
    QComboBox* cbFill_ = nullptr;

    QCheckBox* showWeightsCheck_ = nullptr;

    QCheckBox* cbXComputer_ = nullptr;
    QCheckBox* cbOComputer_ = nullptr;

    QLabel* lblInfo_ = nullptr;

    UiLang lang_ = UiLang::RU;
    bool computerStepScheduled_ = false;

    int gamesPlayed_ = 0;
    int xWins_ = 0;
    int oWins_ = 0;
    int draws_ = 0;
};

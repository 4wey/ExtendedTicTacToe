#pragma once

#include <QMainWindow>

class BoardWidget;
class StatsDialog;
class RulesDialog;

class QPushButton;
class QComboBox;
class QLabel;

#include "game/game_engine.h"

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

    void onCellClicked(int r, int c);

private:
    void buildUi();
    void retranslateUi();

    void syncUiToEngine();
    void refreshBoard();
    void updateInfoLabel();
    void updateRulesButton();

    QString stripeInfoText() const;

private:
    BoardWidget* board_ = nullptr;

    QPushButton* btnNewGame_ = nullptr;
    QPushButton* btnStats_   = nullptr;
    QPushButton* btnRules_   = nullptr;

    QComboBox* cbLanguage_ = nullptr;
    QComboBox* cbMode_     = nullptr;
    QComboBox* cbFill_     = nullptr;

    QLabel* lblLang_ = nullptr;
    QLabel* lblMode_ = nullptr;
    QLabel* lblFill_ = nullptr;
    QLabel* lblInfo_ = nullptr;

    StatsDialog* statsDlg_ = nullptr;
    RulesDialog* rulesDlg_ = nullptr;

    UiLang lang_ = UiLang::RU;

    int gamesPlayed_ = 0;
    int xWins_ = 0;
    int oWins_ = 0;
    int draws_ = 0;

    GameEngine engine_;
};

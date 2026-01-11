#include "mainwindow.h"
#include "widgets/boardwidget.h"
#include "statsdialog.h"
#include "ui/rulesdialog.h"

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QString>

static QString markText(int p) { return (p == 1) ? "X" : "O"; }

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    buildUi();
    retranslateUi();

    cbLanguage_->setCurrentIndex(0);
    cbMode_->setCurrentIndex(0);
    cbFill_->setCurrentIndex(0);

    connect(btnNewGame_, &QPushButton::clicked, this, &MainWindow::onNewGame);
    connect(btnStats_, &QPushButton::clicked, this, &MainWindow::onShowStats);
    connect(btnRules_, &QPushButton::clicked, this, &MainWindow::onShowRules);

    connect(cbLanguage_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLanguageChanged);
    connect(cbMode_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);
    connect(cbFill_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFillModeChanged);

    connect(board_, &BoardWidget::cellClicked, this, &MainWindow::onCellClicked);

    setMinimumSize(900, 600);

    syncUiToEngine();
    engine_.startNewGame();
    refreshBoard();
    updateInfoLabel();
    updateRulesButton();
}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi() {
    auto* root = new QWidget(this);
    setCentralWidget(root);

    auto* leftPanel = new QVBoxLayout();
    leftPanel->setSpacing(12);

    btnNewGame_ = new QPushButton(root);
    btnStats_   = new QPushButton(root);
    btnRules_   = new QPushButton(root);

    lblLang_ = new QLabel(root);
    cbLanguage_ = new QComboBox(root);
    cbLanguage_->addItem("Русский");
    cbLanguage_->addItem("English");

    lblMode_ = new QLabel(root);
    cbMode_ = new QComboBox(root);
    cbMode_->addItem("Classic");
    cbMode_->addItem("Score");

    lblFill_ = new QLabel(root);
    cbFill_ = new QComboBox(root);
    cbFill_->addItem("Free");
    cbFill_->addItem("Top-down");
    cbFill_->addItem("Left-right");
    cbFill_->addItem("Random row");
    cbFill_->addItem("Random col");
    cbFill_->addItem("Random row OR col");

    lblInfo_ = new QLabel(root);
    lblInfo_->setWordWrap(true);

    auto* gbSettings = new QGroupBox(root);
    auto* settingsLayout = new QVBoxLayout(gbSettings);
    settingsLayout->addWidget(lblLang_);
    settingsLayout->addWidget(cbLanguage_);
    settingsLayout->addSpacing(8);
    settingsLayout->addWidget(lblMode_);
    settingsLayout->addWidget(cbMode_);
    settingsLayout->addSpacing(8);
    settingsLayout->addWidget(lblFill_);
    settingsLayout->addWidget(cbFill_);

    leftPanel->addWidget(btnNewGame_);
    leftPanel->addWidget(btnStats_);
    leftPanel->addWidget(btnRules_);
    leftPanel->addWidget(gbSettings);
    leftPanel->addWidget(lblInfo_);
    leftPanel->addStretch(1);

    board_ = new BoardWidget(root);
    board_->setBoardSize(3);

    auto* mainLayout = new QHBoxLayout(root);
    mainLayout->addLayout(leftPanel, 0);
    mainLayout->addWidget(board_, 1);
}

void MainWindow::retranslateUi() {
    if (lang_ == UiLang::RU) {
        setWindowTitle("Крестики-нолики");
        btnNewGame_->setText("Новая игра");
        btnStats_->setText("Статистика");
        btnRules_->setText("Правила");

        lblLang_->setText("Язык");
        lblMode_->setText("Режим");
        lblFill_->setText("Заполнение (только Score)");

        cbLanguage_->setItemText(0, "Русский");
        cbLanguage_->setItemText(1, "English");

        cbMode_->setItemText(0, "Классика 3×3");
        cbMode_->setItemText(1, "Score: 10×10, линия 5, 60 ходов");

        cbFill_->setItemText(0, "Свободно");
        cbFill_->setItemText(1, "Сверху-вниз (по строкам)");
        cbFill_->setItemText(2, "Слева-направо (по столбцам)");
        cbFill_->setItemText(3, "Случайная строка");
        cbFill_->setItemText(4, "Случайный столбец");
        cbFill_->setItemText(5, "Случайная строка ИЛИ столбец");
    } else {
        setWindowTitle("Tic-Tac-Toe");
        btnNewGame_->setText("New Game");
        btnStats_->setText("Statistics");
        btnRules_->setText("Rules");

        lblLang_->setText("Language");
        lblMode_->setText("Mode");
        lblFill_->setText("Filling (Score only)");

        cbMode_->setItemText(0, "Classic 3×3");
        cbMode_->setItemText(1, "Score: 10×10, line 5, 60 moves");

        cbFill_->setItemText(0, "Free");
        cbFill_->setItemText(1, "Top-down (rows)");
        cbFill_->setItemText(2, "Left-right (cols)");
        cbFill_->setItemText(3, "Random row");
        cbFill_->setItemText(4, "Random col");
        cbFill_->setItemText(5, "Random row OR col");
    }
}

void MainWindow::syncUiToEngine() {
    GameMode mode = (cbMode_->currentIndex() == 0) ? GameMode::Classic3x3 : GameMode::Score10x10;
    engine_.setMode(mode);

    FillMode fill = static_cast<FillMode>(cbFill_->currentIndex());
    engine_.setFillMode(fill);

    bool score = (mode == GameMode::Score10x10);
    cbFill_->setEnabled(score);
    lblFill_->setEnabled(score);

    board_->setBoardSize(engine_.boardSize());
}

QString MainWindow::stripeInfoText() const {
    if (engine_.mode() != GameMode::Score10x10) return "";

    const int ar = engine_.activeRow();
    const int ac = engine_.activeCol();

    FillMode f = engine_.fillMode();

    if (f == FillMode::TopDownRows) {
        return (lang_ == UiLang::RU) ? QString(" | Активная строка: %1").arg(ar)
                                     : QString(" | Active row: %1").arg(ar);
    }
    if (f == FillMode::LeftRightCols) {
        return (lang_ == UiLang::RU) ? QString(" | Активный столбец: %1").arg(ac)
                                     : QString(" | Active col: %1").arg(ac);
    }
    if (f == FillMode::RandomRow) {
        return (lang_ == UiLang::RU) ? QString(" | Случайная строка: %1").arg(ar)
                                     : QString(" | Random row: %1").arg(ar);
    }
    if (f == FillMode::RandomCol) {
        return (lang_ == UiLang::RU) ? QString(" | Случайный столбец: %1").arg(ac)
                                     : QString(" | Random col: %1").arg(ac);
    }
    if (f == FillMode::RandomRowOrCol) {
        return (lang_ == UiLang::RU) ? QString(" | Случ. строка: %1, столбец: %2").arg(ar).arg(ac)
                                     : QString(" | Rand row: %1, col: %2").arg(ar).arg(ac);
    }
    return "";
}

void MainWindow::refreshBoard() {
    const int N = engine_.boardSize();
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            int v = engine_.cellOwner(r, c);
            QString t = (v == 0) ? "" : markText(v);
            bool enabled = engine_.isMoveAllowed(r, c);
            board_->setCellText(r, c, t, enabled);
        }
    }
}

void MainWindow::updateInfoLabel() {
    if (!engine_.isActive()) return;

    if (engine_.mode() == GameMode::Classic3x3) {
        lblInfo_->setText((lang_ == UiLang::RU)
            ? QString("Классика: ход %1.").arg(markText(engine_.currentPlayer()))
            : QString("Classic: %1 to move.").arg(markText(engine_.currentPlayer()))
        );
        return;
    }

    ScoreSnapshot s = engine_.currentScore();
    if (lang_ == UiLang::RU) {
        lblInfo_->setText(
            QString("Score | Ход: %1 | Осталось ходов: %2%3\nЛинии: X=%4 O=%5 | Потрачено: X=%6 O=%7 | Итог: X=%8 O=%9")
                .arg(markText(engine_.currentPlayer()))
                .arg(engine_.movesLeft())
                .arg(stripeInfoText())
                .arg(s.xLine).arg(s.oLine)
                .arg(s.xSpent).arg(s.oSpent)
                .arg(s.xTotal).arg(s.oTotal)
        );
    } else {
        lblInfo_->setText(
            QString("Score | Turn: %1 | Left: %2%3\nLines: X=%4 O=%5 | Spent: X=%6 O=%7 | Total: X=%8 O=%9")
                .arg(markText(engine_.currentPlayer()))
                .arg(engine_.movesLeft())
                .arg(stripeInfoText())
                .arg(s.xLine).arg(s.oLine)
                .arg(s.xSpent).arg(s.oSpent)
                .arg(s.xTotal).arg(s.oTotal)
        );
    }
}

void MainWindow::updateRulesButton() {
    btnRules_->setVisible(engine_.mode() == GameMode::Score10x10);
}

void MainWindow::onNewGame() {
    gamesPlayed_++;

    syncUiToEngine();
    engine_.startNewGame();

    refreshBoard();
    updateInfoLabel();
    updateRulesButton();
}

void MainWindow::onCellClicked(int r, int c) {
    if (!engine_.isActive()) return;

    MoveOutcome out = engine_.applyMove(r, c);
    if (!out.accepted) return;

    refreshBoard();

    if (out.finished) {
        if (engine_.mode() == GameMode::Classic3x3) {
            if (out.classicWinner == 1) xWins_++;
            else if (out.classicWinner == -1) oWins_++;
            else draws_++;

            lblInfo_->setText((lang_ == UiLang::RU)
                ? (out.classicWinner == 0 ? "Ничья." : QString("Победа %1!").arg(markText(out.classicWinner)))
                : (out.classicWinner == 0 ? "Draw." : QString("%1 wins!").arg(markText(out.classicWinner)))
            );
        } else {
            ScoreSnapshot s = out.score;
            if (s.xTotal > s.oTotal) xWins_++;
            else if (s.oTotal > s.xTotal) oWins_++;
            else draws_++;

            QString result = (s.xTotal > s.oTotal) ? "X" : (s.oTotal > s.xTotal ? "O" : "D");

            if (lang_ == UiLang::RU) {
                lblInfo_->setText(
                    (result == "D" ? "Ничья." : QString("Победа %1!").arg(result)) +
                    QString("\nЛинии: X=%1, O=%2").arg(s.xLine).arg(s.oLine) +
                    QString("\nПотрачено: X=%1, O=%2").arg(s.xSpent).arg(s.oSpent) +
                    QString("\nИтог: X=%1, O=%2").arg(s.xTotal).arg(s.oTotal)
                );
            } else {
                lblInfo_->setText(
                    (result == "D" ? "Draw." : QString("%1 wins!").arg(result)) +
                    QString("\nLines: X=%1, O=%2").arg(s.xLine).arg(s.oLine) +
                    QString("\nSpent: X=%1, O=%2").arg(s.xSpent).arg(s.oSpent) +
                    QString("\nTotal: X=%1, O=%2").arg(s.xTotal).arg(s.oTotal)
                );
            }
        }
        return;
    }

    updateInfoLabel();
}

void MainWindow::onShowStats() {
    if (!statsDlg_) statsDlg_ = new StatsDialog(this);
    statsDlg_->setStats(gamesPlayed_, xWins_, oWins_, draws_, lang_ == UiLang::RU);
    statsDlg_->exec();
}

void MainWindow::onShowRules() {
    if (engine_.mode() == GameMode::Classic3x3) return;

    if (!rulesDlg_) rulesDlg_ = new RulesDialog(this);
    rulesDlg_->setRules(engine_.mode(), engine_.fillMode(), engine_.config(), lang_ == UiLang::RU);
    rulesDlg_->exec();
}

void MainWindow::onLanguageChanged(int index) {
    lang_ = (index == 0) ? UiLang::RU : UiLang::EN;
    retranslateUi();
    updateInfoLabel();
}

void MainWindow::onModeChanged(int) {
    syncUiToEngine();
    engine_.startNewGame();
    refreshBoard();
    updateInfoLabel();
    updateRulesButton();
}

void MainWindow::onFillModeChanged(int) {
    syncUiToEngine();
    // не стартуем заново — просто обновим доступность клеток
    refreshBoard();
    updateInfoLabel();
}

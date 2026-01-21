#include "mainwindow.h"
#include "widgets/boardwidget.h"
#include "statsdialog.h"
#include "ui/rulesdialog.h"

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QString>

static QString markText(int p) {
    if (p == 1) return "X";
    if (p == -1) return "O";
    return "";
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    buildUi();
    retranslateUi();

    cbLanguage_->setCurrentIndex(0);
    cbMode_->setCurrentIndex(0);
    cbFill_->setCurrentIndex(0);

    cbXComputer_->setChecked(false);
    cbOComputer_->setChecked(true);

    connect(btnNewGame_, &QPushButton::clicked, this, &MainWindow::onNewGame);
    connect(btnStats_, &QPushButton::clicked, this, &MainWindow::onShowStats);
    connect(btnRules_, &QPushButton::clicked, this, &MainWindow::onShowRules);

    connect(cbLanguage_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLanguageChanged);
    connect(cbMode_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);
    connect(cbFill_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFillModeChanged);

    connect(cbXComputer_, &QCheckBox::toggled, this, &MainWindow::onPlayerTypeChanged);
    connect(cbOComputer_, &QCheckBox::toggled, this, &MainWindow::onPlayerTypeChanged);

    connect(board_, &BoardWidget::cellClicked, this, &MainWindow::onCellClicked);

    setMinimumSize(900, 600);

    syncUiToEngine();
    engine_.startNewGame();
    refreshBoard();
    updateInfoLabel();
    updateRulesButton();
    maybeScheduleComputer();
    showWeightsCheck_ = new QCheckBox(tr("Показать веса клеток"), this);
    showWeightsCheck_->setChecked(false);
    showWeightsCheck_->setVisible(false);

}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi() {
    QWidget* root = new QWidget(this);
    setCentralWidget(root);

    QHBoxLayout* layout = new QHBoxLayout(root);

    QVBoxLayout* leftPanel = new QVBoxLayout();

    btnNewGame_ = new QPushButton(root);
    btnStats_ = new QPushButton(root);
    btnRules_ = new QPushButton(root);

    leftPanel->addWidget(btnNewGame_);
    leftPanel->addWidget(btnStats_);
    leftPanel->addWidget(btnRules_);

    leftPanel->addSpacing(12);

    QGroupBox* grpSettings = new QGroupBox(root);
    QVBoxLayout* settingsLayout = new QVBoxLayout(grpSettings);

    lblLanguage_ = new QLabel(grpSettings);
    cbLanguage_ = new QComboBox(grpSettings);
    cbLanguage_->addItem("RU");
    cbLanguage_->addItem("EN");

    lblMode_ = new QLabel(grpSettings);
    cbMode_ = new QComboBox(grpSettings);
    cbMode_->addItem("Classic 3x3");
    cbMode_->addItem("Score 10x10");

    lblFill_ = new QLabel(grpSettings);
    cbFill_ = new QComboBox(grpSettings);
    cbFill_->addItem("Free");
    cbFill_->addItem("Top-down rows");
    cbFill_->addItem("Left-right cols");
    cbFill_->addItem("Random row");
    cbFill_->addItem("Random col");
    cbFill_->addItem("Random row OR col");

    cbXComputer_ = new QCheckBox(root);
    cbOComputer_ = new QCheckBox(root);

    settingsLayout->addWidget(lblLanguage_);
    settingsLayout->addWidget(cbLanguage_);
    settingsLayout->addSpacing(8);

    settingsLayout->addWidget(lblMode_);
    settingsLayout->addWidget(cbMode_);
    settingsLayout->addSpacing(8);

    settingsLayout->addWidget(lblFill_);
    settingsLayout->addWidget(cbFill_);

    settingsLayout->addSpacing(8);
    settingsLayout->addWidget(cbXComputer_);
    settingsLayout->addWidget(cbOComputer_);

    grpSettings->setLayout(settingsLayout);
    leftPanel->addWidget(grpSettings);

    leftPanel->addStretch(1);

    lblInfo_ = new QLabel(root);
    lblInfo_->setWordWrap(true);
    leftPanel->addWidget(lblInfo_);

    board_ = new BoardWidget(root);

    layout->addLayout(leftPanel, 0);
    layout->addWidget(board_, 1);
}

void MainWindow::retranslateUi() {
    if (cbLanguage_->currentIndex() == 0) lang_ = UiLang::RU;
    else lang_ = UiLang::EN;

    if (lang_ == UiLang::RU) {
        setWindowTitle("TicTacToe");
        btnNewGame_->setText("Новая игра");
        btnStats_->setText("Статистика");
        btnRules_->setText("Правила");

        lblLanguage_->setText("Язык");
        lblMode_->setText("Режим");
        lblFill_->setText("Заполнение (только Score)");
        cbXComputer_->setText("X — компьютер");
        cbOComputer_->setText("O — компьютер");
    } else {
        setWindowTitle("TicTacToe");
        btnNewGame_->setText("New game");
        btnStats_->setText("Stats");
        btnRules_->setText("Rules");

        lblLanguage_->setText("Language");
        lblMode_->setText("Mode");
        lblFill_->setText("Fill (Score only)");
        cbXComputer_->setText("X is computer");
        cbOComputer_->setText("O is computer");
    }

    updateInfoLabel();
    updateRulesButton();
}

void MainWindow::syncUiToEngine() {
    GameMode mode = static_cast<GameMode>(cbMode_->currentIndex());
    engine_.setMode(mode);

    FillMode fill = static_cast<FillMode>(cbFill_->currentIndex());
    engine_.setFillMode(fill);

    engine_.setPlayerTypeX(cbXComputer_->isChecked() ? PlayerType::Computer : PlayerType::Human);
    engine_.setPlayerTypeO(cbOComputer_->isChecked() ? PlayerType::Computer : PlayerType::Human);
}

void MainWindow::refreshBoard() {
    const int N = engine_.boardSize();
    board_->setBoardSize(N);

    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            const int owner = engine_.cellOwner(r, c);
            QString text = owner == 0 ? "" : markText(owner);
            bool enabled = engine_.isMoveAllowed(r, c) && !engine_.isCurrentPlayerComputer();
            board_->setCellText(r, c, text, enabled);
        }
    }
}

QString MainWindow::stripeInfoText() const {
    if (engine_.mode() != GameMode::Score10x10) return "";

    const int r = engine_.activeRow();
    const int c = engine_.activeCol();

    if (engine_.fillMode() == FillMode::TopDownRows) {
        return (lang_ == UiLang::RU)
            ? QString("Активная строка: %1").arg(r + 1)
            : QString("Active row: %1").arg(r + 1);
    }
    if (engine_.fillMode() == FillMode::LeftRightCols) {
        return (lang_ == UiLang::RU)
            ? QString("Активный столбец: %1").arg(c + 1)
            : QString("Active col: %1").arg(c + 1);
    }
    if (engine_.fillMode() == FillMode::RandomRow) {
        return (lang_ == UiLang::RU)
            ? QString("Случайная строка: %1").arg(r + 1)
            : QString("Random row: %1").arg(r + 1);
    }
    if (engine_.fillMode() == FillMode::RandomCol) {
        return (lang_ == UiLang::RU)
            ? QString("Случайный столбец: %1").arg(c + 1)
            : QString("Random col: %1").arg(c + 1);
    }
    if (engine_.fillMode() == FillMode::RandomRowOrCol) {
        return (lang_ == UiLang::RU)
            ? QString("Случайная полоса: row=%1 col=%2").arg(r + 1).arg(c + 1)
            : QString("Random stripe: row=%1 col=%2").arg(r + 1).arg(c + 1);
    }

    return "";
}

void MainWindow::updateInfoLabel() {
    if (!engine_.isActive()) return;

    if (engine_.mode() == GameMode::Classic3x3) {
        lblInfo_->setText(
            (lang_ == UiLang::RU)
                ? QString("Ход: %1").arg(markText(engine_.currentPlayer()))
                : QString("Turn: %1").arg(markText(engine_.currentPlayer()))
        );
        return;
    }

    ScoreSnapshot s = engine_.currentScore();
    lblInfo_->setText(
        (lang_ == UiLang::RU ? "Score mode\n" : "Score mode\n") +
        stripeInfoText() +
        QString("\nЛинии: X=%1, O=%2").arg(s.xLine).arg(s.oLine) +
        QString("\nПотрачено: X=%1, O=%2").arg(s.xSpent).arg(s.oSpent) +
        QString("\nИтог: X=%1, O=%2").arg(s.xTotal).arg(s.oTotal) +
        QString("\n\n") +
        (lang_ == UiLang::RU ? QString("Ход: %1").arg(markText(engine_.currentPlayer()))
                            : QString("Turn: %1").arg(markText(engine_.currentPlayer())))
    );
}

void MainWindow::updateRulesButton() {
    btnRules_->setEnabled(engine_.mode() != GameMode::Classic3x3);
}

void MainWindow::maybeScheduleComputer() {
    if (computerStepScheduled_) return;
    if (!engine_.isActive()) return;
    if (!engine_.isCurrentPlayerComputer()) return;

    computerStepScheduled_ = true;
    QTimer::singleShot(150, this, &MainWindow::doComputerStep);
}

void MainWindow::doComputerStep() {
    computerStepScheduled_ = false;

    if (!engine_.isActive()) return;
    if (!engine_.isCurrentPlayerComputer()) return;

    MoveOutcome out = engine_.doComputerMove();
    if (!out.accepted) return;

    refreshBoard();

    if (out.finished) {
        applyFinishedResult(out);
        return;
    }

    updateInfoLabel();
    maybeScheduleComputer();
}

void MainWindow::applyFinishedResult(const MoveOutcome& out) {
    if (engine_.mode() == GameMode::Classic3x3) {
        if (out.classicWinner == 1) xWins_++;
        else if (out.classicWinner == -1) oWins_++;
        else draws_++;

        lblInfo_->setText((lang_ == UiLang::RU)
            ? (out.classicWinner == 0 ? "Ничья." : QString("Победа %1!").arg(markText(out.classicWinner)))
            : (out.classicWinner == 0 ? "Draw." : QString("%1 wins!").arg(markText(out.classicWinner)))
        );
        return;
    }

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

void MainWindow::onPlayerTypeChanged() {
    syncUiToEngine();
    refreshBoard();
    updateInfoLabel();
    maybeScheduleComputer();
}

void MainWindow::onNewGame() {
    gamesPlayed_++;

    syncUiToEngine();
    engine_.startNewGame();

    refreshBoard();
    updateInfoLabel();
    updateRulesButton();
    maybeScheduleComputer();
}

void MainWindow::onCellClicked(int r, int c) {
    if (!engine_.isActive()) return;

    MoveOutcome out = engine_.applyMove(r, c);
    if (!out.accepted) return;

    refreshBoard();

    if (out.finished) {
        applyFinishedResult(out);
        return;
    }

    updateInfoLabel();
    maybeScheduleComputer();
}

void MainWindow::onShowStats() {
    StatsDialog dlg(this);
    dlg.setStats(gamesPlayed_, xWins_, oWins_, draws_, lang_ == UiLang::RU);
    dlg.exec();
}

void MainWindow::onShowRules() {
    RulesDialog dlg(this);
    dlg.setMode(engine_.mode(), lang_ == UiLang::RU);
    dlg.exec();
}

void MainWindow::onLanguageChanged(int) {
    retranslateUi();
}

void MainWindow::onModeChanged(int) {
    onNewGame();
    maybeScheduleComputer();
}

void MainWindow::onFillModeChanged(int) {
    if (engine_.mode() != GameMode::Score10x10) return;
    syncUiToEngine();
    engine_.startNewGame();
    refreshBoard();
    updateInfoLabel();
    maybeScheduleComputer();
}

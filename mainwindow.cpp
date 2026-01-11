#include "mainwindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>

#include "widgets/boardwidget.h"
#include "ui/rulesdialog.h"
#include "statsdialog.h"

static bool isScoreMode(GameMode m) {
    return m == GameMode::Score10x10;
}

static FillMode fillModeFromIndex(int idx) {
    switch (idx) {
        case 0: return FillMode::Free;
        case 1: return FillMode::TopDownRows;
        case 2: return FillMode::LeftRightCols;
        case 3: return FillMode::RandomRow;
        case 4: return FillMode::RandomCol;
        case 5: return FillMode::RandomRowOrCol;
        default: return FillMode::Free;
    }
}

static int fillIndexFromMode(FillMode m) {
    switch (m) {
        case FillMode::Free: return 0;
        case FillMode::TopDownRows: return 1;
        case FillMode::LeftRightCols: return 2;
        case FillMode::RandomRow: return 3;
        case FillMode::RandomCol: return 4;
        case FillMode::RandomRowOrCol: return 5;
        default: return 0;
    }
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupUi();

    // режим по умолчанию
    engine_.setMode(GameMode::Classic3x3);
    engine_.setFillMode(FillMode::Free);

    applyModeUi();
    engine_.startNewGame();
    refreshBoardAll();
    refreshStatus();
}

void MainWindow::setupUi() {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);

    // Верхняя панель: режим + fill + показать веса + кнопки
    auto* topRow = new QHBoxLayout();
    topRow->setSpacing(10);

    modeCombo_ = new QComboBox(this);
    modeCombo_->addItem("Обычные 3×3", (int)GameMode::Classic3x3);
    modeCombo_->addItem("Score 10×10", (int)GameMode::Score10x10);

    fillCombo_ = new QComboBox(this);
    fillCombo_->addItem("Без ограничений", (int)FillMode::Free);
    fillCombo_->addItem("Сверху вниз (строки)", (int)FillMode::TopDownRows);
    fillCombo_->addItem("Слева направо (столбцы)", (int)FillMode::LeftRightCols);
    fillCombo_->addItem("Случайная строка", (int)FillMode::RandomRow);
    fillCombo_->addItem("Случайный столбец", (int)FillMode::RandomCol);
    fillCombo_->addItem("Случайная строка/столбец", (int)FillMode::RandomRowOrCol);

    showWeightsBox_ = new QCheckBox("Показать веса", this);
    showWeightsBox_->setChecked(false);

    newGameBtn_ = new QPushButton("Новая игра", this);
    rulesBtn_ = new QPushButton("Правила", this);

    topRow->addWidget(new QLabel("Режим:", this));
    topRow->addWidget(modeCombo_, 1);

    topRow->addWidget(new QLabel("Ограничение:", this));
    topRow->addWidget(fillCombo_, 1);

    topRow->addWidget(showWeightsBox_);
    topRow->addStretch(1);
    topRow->addWidget(rulesBtn_);
    topRow->addWidget(newGameBtn_);

    root->addLayout(topRow);

    // Статусы
    statusLabel_ = new QLabel(this);
    scoreLabel_ = new QLabel(this);
    statusLabel_->setWordWrap(true);
    scoreLabel_->setWordWrap(true);

    root->addWidget(statusLabel_);
    root->addWidget(scoreLabel_);

    // Поле
    board_ = new BoardWidget(this);

    // Иконки генерим сами (чтобы всегда работало)
    board_->setIcons(makeXIcon(256), makeOIcon(256));

    root->addWidget(board_, 0, Qt::AlignCenter);

    // Сигналы
    connect(modeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);
    connect(fillCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFillModeChanged);
    connect(showWeightsBox_, &QCheckBox::toggled,
            this, &MainWindow::onShowWeightsToggled);
    connect(newGameBtn_, &QPushButton::clicked,
            this, &MainWindow::onNewGameClicked);
    connect(rulesBtn_, &QPushButton::clicked,
            this, &MainWindow::onRulesClicked);
    connect(board_, &BoardWidget::cellClicked,
            this, &MainWindow::onCellClicked);

    setWindowTitle("TicTacToe");
    resize(900, 900);
}

void MainWindow::onModeChanged(int index) {
    const auto mode = (GameMode)modeCombo_->itemData(index).toInt();
    engine_.setMode(mode);

    // при смене режима — сброс ограничений/весов/галочки
    if (isScoreMode(mode)) {
        engine_.setFillMode(fillModeFromIndex(fillCombo_->currentIndex()));
    } else {
        engine_.setFillMode(FillMode::Free);
    }

    showWeightsBox_->blockSignals(true);
    showWeightsBox_->setChecked(false);
    showWeightsBox_->blockSignals(false);

    applyModeUi();

    engine_.startNewGame();
    refreshBoardAll();
    refreshStatus();
}

void MainWindow::onFillModeChanged(int index) {
    Q_UNUSED(index);
    applyFillUi();
    refreshBoardAll();
    refreshStatus();
}

void MainWindow::onShowWeightsToggled(bool on) {
    board_->setShowWeights(on);
}

void MainWindow::onNewGameClicked() {
    engine_.startNewGame();
    refreshBoardAll();
    refreshStatus();
}

void MainWindow::onRulesClicked() {
    const GameMode m = engine_.mode();

    if (!isScoreMode(m)) {
        // по твоему требованию: для 3×3 правила можно не показывать
        QMessageBox::information(this, "Правила", "Обычные крестики-нолики 3×3.");
        return;
    }

    QString rules;
    rules += "Score mode (10×10)\n\n";
    rules += "1) Ходы:\n";
    rules += "   • X ходит первым и платит только чётные стоимости: 2,4,6,8,...\n";
    rules += "   • O платит только нечётные стоимости: 3,5,7,9,...\n\n";
    rules += "2) Поле имеет веса клеток (целые числа).\n";
    rules += "   Очки линий считаются по сумме весов клеток, занятых линией.\n";
    rules += "   (Линии считаем длиной 5 по горизонтали/вертикали/диагоналям.)\n\n";
    rules += "3) Итоговые очки:\n";
    rules += "   total = lineScore - spent\n\n";
    rules += "4) Ограничение ходов:\n";
    rules += "   • Без ограничений\n";
    rules += "   • Сверху вниз (разрешена только первая незаполненная строка)\n";
    rules += "   • Слева направо (разрешён только первый незаполненный столбец)\n";
    rules += "   • Случайная строка / столбец / строка-или-столбец\n";

    RulesDialog dlg(this);
    dlg.setWindowTitle("Правила режима");
    dlg.setRulesText(rules);
    dlg.exec();
}

void MainWindow::onCellClicked(int r, int c) {
    if (!engine_.isActive()) return;

    if (!engine_.isMoveAllowed(r, c)) {
        // тихо игнорируем — или можно показать подсказку
        return;
    }

    MoveOutcome out = engine_.applyMove(r, c);

    // обновим весь борд (быстрее по разработке, чем микропатчить)
    refreshBoardAll();
    refreshStatus();

    finishIfNeeded(out);
}

void MainWindow::finishIfNeeded(const MoveOutcome& out) {
    if (!out.finished) return;

    const GameMode m = engine_.mode();

    if (!isScoreMode(m)) {
        if (out.classicWinner == 1) {
            QMessageBox::information(this, "Игра окончена", "Победил X!");
        } else if (out.classicWinner == -1) {
            QMessageBox::information(this, "Игра окончена", "Победил O!");
        } else {
            QMessageBox::information(this, "Игра окончена", "Ничья.");
        }
        return;
    }

    // Score mode итог
    const auto s = out.score;
    QString msg;
    msg += "Score mode завершён.\n\n";
    msg += QString("X: линии=%1, потрачено=%2, итог=%3\n").arg(s.xLine).arg(s.xSpent).arg(s.xTotal);
    msg += QString("O: линии=%1, потрачено=%2, итог=%3\n").arg(s.oLine).arg(s.oSpent).arg(s.oTotal);

    if (s.xTotal > s.oTotal) msg += "\nПобедил X!";
    else if (s.oTotal > s.xTotal) msg += "\nПобедил O!";
    else msg += "\nНичья.";

    QMessageBox::information(this, "Игра окончена", msg);
}

void MainWindow::applyFillUi() {
    if (!isScoreMode(engine_.mode())) {
        engine_.setFillMode(FillMode::Free);
        return;
    }
    engine_.setFillMode(fillModeFromIndex(fillCombo_->currentIndex()));
}

void MainWindow::applyModeUi() {
    const GameMode m = engine_.mode();
    const bool score = isScoreMode(m);

    // включаем/выключаем контролы
    fillCombo_->setEnabled(score);
    showWeightsBox_->setEnabled(score);
    rulesBtn_->setEnabled(score);

    // фиксированные размеры клеток (подкрути как нравится)
    if (!score) {
        board_->setCellSizePx(150);  // большие
        board_->setBoardSize(3);
    } else {
        board_->setCellSizePx(58);   // компактные
        board_->setBoardSize(10);
    }

    // сброс отображения весов при смене режима
    board_->setShowWeights(score ? showWeightsBox_->isChecked() : false);

    // синхронизируем fillCombo с текущим fill режима
    if (score) {
        fillCombo_->blockSignals(true);
        fillCombo_->setCurrentIndex(fillIndexFromMode(engine_.fillMode()));
        fillCombo_->blockSignals(false);
    } else {
        fillCombo_->blockSignals(true);
        fillCombo_->setCurrentIndex(0);
        fillCombo_->blockSignals(false);
    }
}

void MainWindow::refreshBoardAll() {
    const int N = engine_.boardSize();

    // веса (для score)
    if (isScoreMode(engine_.mode())) {
        QVector<int> w;
        w.resize(N * N);
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                w[r * N + c] = engine_.cellWeight(r, c);
            }
        }
        board_->setWeights(w);
    } else {
        // в классике веса = 0
        QVector<int> w;
        w.fill(0, N * N);
        board_->setWeights(w);
    }

    // X/O + enabled
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            const int owner = engine_.cellOwner(r, c);
            const bool enabled = engine_.isActive() && engine_.isMoveAllowed(r, c);
            board_->setCellOwner(r, c, owner, enabled);
        }
    }
}

void MainWindow::refreshStatus() {
    const GameMode m = engine_.mode();
    const int player = engine_.currentPlayer();
    const QString p = (player == 1 ? "X" : "O");

    QString status;
    if (!engine_.isActive()) {
        status = "Игра не активна. Нажми «Новая игра».";
    } else {
        status = QString("Ход: %1 | Осталось ходов: %2").arg(p).arg(engine_.movesLeft());
    }

    if (isScoreMode(m)) {
        const int ar = engine_.activeRow();
        const int ac = engine_.activeCol();

        if (engine_.fillMode() == FillMode::TopDownRows && ar >= 0) {
            status += QString(" | Активная строка: %1").arg(ar + 1);
        } else if (engine_.fillMode() == FillMode::LeftRightCols && ac >= 0) {
            status += QString(" | Активный столбец: %1").arg(ac + 1);
        } else if ((engine_.fillMode() == FillMode::RandomRow || engine_.fillMode() == FillMode::RandomRowOrCol) && ar >= 0) {
            status += QString(" | Случайная строка: %1").arg(ar + 1);
        }
        if ((engine_.fillMode() == FillMode::RandomCol || engine_.fillMode() == FillMode::RandomRowOrCol) && ac >= 0) {
            status += QString(" | Случайный столбец: %1").arg(ac + 1);
        }
    }

    statusLabel_->setText(status);

    // score подпись
    if (!isScoreMode(m)) {
        scoreLabel_->setText("");
        return;
    }

    const auto s = engine_.currentScore();
    const QString scoreText =
        QString("X: линии=%1, потрачено=%2, итог=%3   |   O: линии=%4, потрачено=%5, итог=%6")
            .arg(s.xLine).arg(s.xSpent).arg(s.xTotal)
            .arg(s.oLine).arg(s.oSpent).arg(s.oTotal);

    scoreLabel_->setText(scoreText);
}

QIcon MainWindow::makeXIcon(int px) {
    QPixmap pm(px, px);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QColor(30, 30, 30));
    pen.setWidth(qMax(8, px / 10));
    pen.setCapStyle(Qt::RoundCap);
    p.setPen(pen);

    const int m = qMax(20, px / 6);
    p.drawLine(m, m, px - m, px - m);
    p.drawLine(px - m, m, m, px - m);

    return QIcon(pm);
}

QIcon MainWindow::makeOIcon(int px) {
    QPixmap pm(px, px);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QColor(30, 30, 30));
    pen.setWidth(qMax(8, px / 10));
    pen.setCapStyle(Qt::RoundCap);
    p.setPen(pen);

    const int m = qMax(20, px / 6);
    p.drawEllipse(QRect(m, m, px - 2 * m, px - 2 * m));

    return QIcon(pm);
}

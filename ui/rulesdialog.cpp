#include "ui/rulesdialog.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

RulesDialog::RulesDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Правила режима");
    setMinimumWidth(400);

    rulesLabel_ = new QLabel(this);
    rulesLabel_->setWordWrap(true);
    rulesLabel_->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    closeBtn_ = new QPushButton("Закрыть", this);
    connect(closeBtn_, &QPushButton::clicked, this, &QDialog::accept);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(rulesLabel_);
    layout->addWidget(closeBtn_);
}

void RulesDialog::setRulesText(const QString& text) {
    rulesLabel_->setText(text);
}

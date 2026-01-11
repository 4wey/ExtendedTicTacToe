#pragma once

#include <QDialog>

class QLabel;
class QPushButton;

class RulesDialog : public QDialog {
    Q_OBJECT
public:
    explicit RulesDialog(QWidget* parent = nullptr);

    // ← ВОТ ЭТОГО МЕТОДА У ТЕБЯ НЕ ХВАТАЛО
    void setRulesText(const QString& text);

private:
    QLabel* rulesLabel_ = nullptr;
    QPushButton* closeBtn_ = nullptr;
};

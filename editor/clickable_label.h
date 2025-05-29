#pragma once

#include <QLabel>
#include <QString>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(const QString& imagePath, QWidget* parent = nullptr);

signals:
    void clicked(const QString& path);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QString path;
};

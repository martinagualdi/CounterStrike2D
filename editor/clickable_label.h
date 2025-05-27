#ifndef CLICKABLE_LABEL_H_
#define CLICKABLE_LABEL_H_

#include <QLabel>
#include <QString>
#include <QMouseEvent>

class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel(const QString& path, QWidget* parent = nullptr) 
        : QLabel(parent), imagePath(path) {}

signals:
    void clicked(const QString& path);

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if(event->button() == Qt::LeftButton)
            emit clicked(imagePath);
        QLabel::mousePressEvent(event);
    }

private:
    QString imagePath;
};

#endif  // CLICKABLE_LABEL_H_

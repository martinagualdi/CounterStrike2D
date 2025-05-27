#pragma once
#include <QWidget>

class TopWidget : public QWidget {
    Q_OBJECT

public:
    explicit TopWidget(QWidget* parent = nullptr);
    void setBackgroundPath(const QString& path);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QString backgroundPath;
};

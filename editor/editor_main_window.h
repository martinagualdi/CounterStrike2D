#ifndef EDITOR_MAIN_WINDOW_H_
#define EDITOR_MAIN_WINDOW_H_

#include <QWidget>
#include <QString>

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

#endif // EDITOR_MAIN_WINDOW_H_

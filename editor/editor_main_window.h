#ifndef EDITOR_MAIN_WINDOW_H_
#define EDITOR_MAIN_WINDOW_H_

#include <QWidget>
#include <QString>
#include "top_widget.h"

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
private:
    void guardarMapa();
    TopWidget* topWidget;
    QString backgroundPath;
};

#endif // EDITOR_MAIN_WINDOW_H_

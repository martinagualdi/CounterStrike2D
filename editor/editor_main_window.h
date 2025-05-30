#ifndef EDITOR_MAIN_WINDOW_H_
#define EDITOR_MAIN_WINDOW_H_

#include <QWidget>
#include <QString>
#include "top_widget.h"

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    void guardarMapa();
    void cargarDesdeYAML(const QString& ruta);
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
private:
    TopWidget* topWidget;
    QString backgroundPath;
};

#endif // EDITOR_MAIN_WINDOW_H_

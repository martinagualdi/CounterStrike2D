#ifndef EDITOR_SELECCION_MAPA_H
#define EDITOR_SELECCION_MAPA_H

#include <QWidget>
#include <QString>

class TopWidget;

class EditorSeleccionMapa : public QWidget {
    Q_OBJECT

public:
    explicit EditorSeleccionMapa(QWidget* parent = nullptr);
    void seleccionarArchivo();

signals:
    void mapaSeleccionado(const QString& rutaYaml);
};

#endif // EDITOR_SELECCION_MAPA_H

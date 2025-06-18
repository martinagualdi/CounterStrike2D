#include "editor_seleccion_mapa.h"
#include "top_widget.h"

#include <yaml-cpp/yaml.h>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QCoreApplication>

EditorSeleccionMapa::EditorSeleccionMapa(QWidget* parent) : QWidget(parent) {
    auto* topWidget = new TopWidget;
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(topWidget);
    setLayout(layout);
}

void EditorSeleccionMapa::seleccionarArchivo() {
    QString ruta = QFileDialog::getOpenFileName(this, "Seleccionar archivo de mapa",
                                            "/var/CounterStrike2D/editor/mapas",
                                            "Archivos YAML (*.yaml)");


    if (!ruta.isEmpty())
        emit mapaSeleccionado(ruta);

    close();
}

#include "editor_main_window.h"
#include "draggable_label.h" 
#include "clickable_label.h" 

#include <yaml-cpp/yaml.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPixmap>
#include <QDir>
#include <QPalette>
#include <QBrush>
#include <QPushButton>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDebug>
#include <QTabWidget>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QApplication>
#include <QGraphicsPixmapItem>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QHBoxLayout* topBarLayout = new QHBoxLayout;
    topBarLayout->setContentsMargins(5, 5, 5, 0);
    topBarLayout->addStretch(); 

    QPushButton* botonZonaInicio = new QPushButton("Marcar Zona de Inicio");
    botonZonaInicio->setFixedSize(180, 30);
    topBarLayout->addWidget(botonZonaInicio);
    connect(botonZonaInicio, &QPushButton::clicked, [this]() {
        topWidget->setDropMode(DropMode::ZONA_INICIO);
    });

    QPushButton* saveButton = new QPushButton("Guardar");
    saveButton->setFixedSize(100, 30);
    topBarLayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::guardarMapa);

    mainLayout->addLayout(topBarLayout);

    // Parte superior (topWidget)
    topWidget = new TopWidget;
    mainLayout->addWidget(topWidget);

    // QTabWidget para las pestañas
    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setFixedHeight(140);  // para incluir scroll + espacio

    // Crear las 4 pestañas
    struct TabInfo {
        QString name;
        QString dirPath;
    };

    TabInfo tabs[] = {
        { "Fondos", "editor/gfx/backgrounds/" },
        { "Azteca", "editor/gfx/aztec/" },
        { "Dust",  "editor/gfx/dust/" },
        { "Inferno", "editor/gfx/inferno/" },
        { "Plantacion de bombas", "editor/gfx/plantacion_bombas/" },
        { "Proteccion anti disparos", "editor/gfx/proteccion_disparos/" },
    };

    for (const auto& tab : tabs) {
        QScrollArea* scrollArea = new QScrollArea;
        scrollArea->setFixedHeight(120);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setWidgetResizable(true);

        QWidget* container = new QWidget;
        QHBoxLayout* hLayout = new QHBoxLayout(container);
        hLayout->setSpacing(5);
        hLayout->setContentsMargins(5, 5, 5, 5);

        QDir dir(tab.dirPath);
        QStringList images = dir.entryList(QStringList() << "*.png" << "*.jpg", QDir::Files);

        for (const QString& imgName : images) {
            QString fullPath = dir.absoluteFilePath(imgName);
            QPixmap fullPixmap(fullPath);

            if (tab.name == "Fondos") {
                auto* clickable = new ClickableLabel(fullPath);
                connect(clickable, &ClickableLabel::clicked, this, [this](const QString& path) {
                    topWidget->setDropMode(DropMode::FONDO);
                    topWidget->setBackgroundPath(path);
                });

                clickable->setPixmap(fullPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                clickable->setFixedSize(100, 100);
                hLayout->addWidget(clickable);
            } else {
                auto* draggable = new DraggableLabel(fullPath);
                connect(draggable, &DraggableLabel::dragStarted, this, [this](const QString&) {
                    topWidget->setDropMode(DropMode::OBJETO);
                });
                draggable->setPixmap(fullPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                draggable->setFixedSize(100, 100);
                hLayout->addWidget(draggable);
            }
        }

        scrollArea->setWidget(container);
        tabWidget->addTab(scrollArea, tab.name);
    }

    mainLayout->addWidget(tabWidget);
    setAcceptDrops(true);
    topWidget->setAcceptDrops(true);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasImage() || event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::guardarMapa() {
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Guardar mapa",
        "",
        "Archivos YAML (*.yaml);;Todos los archivos (*)"
    );

    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".yaml", Qt::CaseInsensitive)) {
        fileName += ".yaml";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo para escribir.";
        return;
    }

    QTextStream out(&file);
    out << "fondo: " << topWidget->getFondoPath() << "\n";
    out << "elementos:\n";

    auto elementos = topWidget->getElementos();
    for (const auto& e : elementos) {
        out << "  - imagen: " << e.path << "\n";
        out << "    x: " << int(e.posicion.x()) << "\n";
        out << "    y: " << int(e.posicion.y()) << "\n";
        out << "    tipo: " << e.tipo << "\n";
    }

    for (const auto& zona : topWidget->getZonas()) {
    out << "  - tipo: " << zona.tipo << "\n";
    out << "    x: " << int(zona.rect.x()) << "\n";
    out << "    y: " << int(zona.rect.y()) << "\n";
    out << "    ancho: " << int(zona.rect.width()) << "\n";
    out << "    alto: " << int(zona.rect.height()) << "\n";
    }


    file.close();
    QApplication::quit();
}

void MainWindow::cargarDesdeYAML(const QString& ruta) {
    YAML::Node root = YAML::LoadFile(ruta.toStdString());

    QString fondo = QString::fromStdString(root["fondo"].as<std::string>());
    topWidget->setDropMode(DropMode::FONDO);
    topWidget->setBackgroundPath(fondo);

    const auto& elementos = root["elementos"];
    for (const auto& elemento : elementos) {
        QString imagen = QString::fromStdString(elemento["imagen"].as<std::string>());
        int x = elemento["x"].as<int>();
        int y = elemento["y"].as<int>();
        topWidget->agregarElemento(imagen, x, y);
    }
}


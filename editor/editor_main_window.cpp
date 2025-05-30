#include "editor_main_window.h"
#include "draggable_label.h" 
#include "clickable_label.h" 

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

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QHBoxLayout* topBarLayout = new QHBoxLayout;
    topBarLayout->setContentsMargins(5, 5, 5, 0);
    topBarLayout->addStretch(); 

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
        { "Infierno", "editor/gfx/inferno/" }
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
        QStringList images = dir.entryList(QStringList() << "*.png", QDir::Files);

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
    qDebug() << "Guardar mapa aún no implementado.";
    // Aquí podrías guardar el contenido del QGraphicsScene, etc.
}
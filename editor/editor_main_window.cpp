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
        { "Objetos", "editor/gfx/tiles/" },
        { "Armas",  "editor/gfx/weapons/" },
        { "Jugadores", "editor/gfx/player/" }
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
            } else if (tab.name == "Objetos") {
                // Dividir la imagen en tiles de 32x32
                const int tileWidth = 32;
                const int tileHeight = 32;
                int cols = fullPixmap.width() / tileWidth;
                int rows = fullPixmap.height() / tileHeight;

                for (int y = 0; y < rows; ++y) {
                    for (int x = 0; x < cols; ++x) {
                        QPixmap tile = fullPixmap.copy(x * tileWidth, y * tileHeight, tileWidth, tileHeight);
                        auto* draggable = new DraggableLabel(tile);
                        draggable->setFixedSize(32, 32);
                        //draggable->setPixmap(tile.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        connect(draggable, &DraggableLabel::dragStarted, this, [this]() {
                            topWidget->setDropMode(DropMode::OBJETO);
                        });
                        hLayout->addWidget(draggable);
                    }
                }
            } else {
                auto* draggable = new DraggableLabel(fullPath);
                connect(draggable, &DraggableLabel::dragStarted, this, [this]() {
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
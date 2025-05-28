#include "editor_main_window.h"
#include "draggable_label.h" 

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
        QStringList images = dir.entryList(QStringList() << "*.png" << "*.jpg" << "*.bmp", QDir::Files);

        for (const QString& imgName : images) {
            QString fullPath = dir.absoluteFilePath(imgName);
            QPixmap pix(fullPath);
            DraggableLabel* label = new DraggableLabel(fullPath);
            label->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            label->setFixedSize(100, 100);
            hLayout->addWidget(label);

            connect(label, &DraggableLabel::clicked, this, [this](const QString& path) {
                topWidget->setBackgroundPath(path);
            });
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



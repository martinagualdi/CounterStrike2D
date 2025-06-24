#include "editor_main_window.h"
#include "draggable_label.h" 
#include "clickable_label.h" 
#include "../common_src/ruta_base.h"

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
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#define TAB_ARMAS "Armas"
#define TAB_FONDOS "Fondos"
#define TAB_DUST "Dust"
#define TAB_INFERNO "Inferno"
#define TAB_AZTECA "Azteca"
#define TAB_PROTECCION "Proteccion anti disparos"

#define PATH_FONDOS "backgrounds/"
#define PATH_ARMAS "weapons/"
#define PATH_DUST "dust/"
#define PATH_INFERNO "inferno/"
#define PATH_AZTECA "aztec/"
#define PATH_PROTECCION "proteccion_disparos/"

#define ARMA_AWP_PERMITIDA "awp_m.bmp"
#define ARMA_AK_PERMITIDA "ak47_m.bmp"
#define ARMA_M3_PERMITIDA "m3_m.bmp"

#define BOTON_ZONA "Marcar Zona"
#define BOTON_GUARDAR "Guardar"
#define BOTON_PINCEL "Activar Pincel Piso"

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QHBoxLayout* topBarLayout = new QHBoxLayout;
    topBarLayout->setContentsMargins(5, 5, 5, 0);
    topBarLayout->addStretch(); 

    QPushButton* botonZonaInicio = new QPushButton(BOTON_ZONA);
    botonZonaInicio->setFixedSize(180, 30);
    topBarLayout->addWidget(botonZonaInicio);
    connect(botonZonaInicio, &QPushButton::clicked, [this]() {
        topWidget->setDropMode(DropMode::ZONA_INICIO);
    });

    QPushButton* botonPincelPiso = new QPushButton(BOTON_PINCEL);
    botonPincelPiso->setFixedSize(180, 30);
    topBarLayout->addWidget(botonPincelPiso);
    connect(botonPincelPiso, &QPushButton::clicked, [this]() {
        topWidget->setDropMode(DropMode::PINCEL_PISO);
        topWidget->activarPincelPiso();
    });

    QPushButton* saveButton = new QPushButton(BOTON_GUARDAR);
    saveButton->setFixedSize(100, 30);
    topBarLayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::guardarMapa);

    mainLayout->addLayout(topBarLayout);

    topWidget = new TopWidget;
    mainLayout->addWidget(topWidget);

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setFixedHeight(140);

    struct TabInfo {
        QString name;
        QString dirPath;
    };

    TabInfo tabs[] = {
        { TAB_FONDOS, QString::fromStdString(RUTA_IMAGENES(PATH_FONDOS)) },
        { TAB_AZTECA, QString::fromStdString(RUTA_IMAGENES(PATH_AZTECA)) },
        { TAB_DUST, QString::fromStdString(RUTA_IMAGENES(PATH_DUST)) },
        { TAB_INFERNO, QString::fromStdString(RUTA_IMAGENES(PATH_INFERNO)) },
        { TAB_ARMAS, QString::fromStdString(RUTA_IMAGENES(PATH_ARMAS)) },
        { TAB_PROTECCION, QString::fromStdString(RUTA_IMAGENES(PATH_PROTECCION)) },
    };


    QStringList armasPermitidas = { ARMA_AK_PERMITIDA, ARMA_M3_PERMITIDA, ARMA_AWP_PERMITIDA};

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
        QStringList filters = (tab.name == TAB_ARMAS)
                            ? QStringList() << "*.bmp"
                            : QStringList() << "*.png" << "*.jpg";
        QStringList images = dir.entryList(filters, QDir::Files);

        for (const QString& imgName : images) {
            if (tab.name == TAB_ARMAS && !armasPermitidas.contains(imgName))
                continue;

            QString fullPath = dir.absoluteFilePath(imgName);

            QPixmap pixmap;
            if (tab.name == TAB_ARMAS) {
                QImage img(fullPath);
                img = img.convertToFormat(QImage::Format_ARGB32);
                QRgb magenta = qRgb(255, 0, 255);

                for (int y = 0; y < img.height(); ++y) {
                    for (int x = 0; x < img.width(); ++x) {
                        QRgb pixel = img.pixel(x, y);
                        if (pixel == magenta)
                            img.setPixelColor(x, y, QColor(0, 0, 0, 0));
                    }
                }
                pixmap = QPixmap::fromImage(img);
            } else {
                pixmap = QPixmap(fullPath);
            }

            if (tab.name == TAB_FONDOS) {
                auto* clickable = new ClickableLabel(fullPath);
                connect(clickable, &ClickableLabel::clicked, this, [this](const QString& path) {
                    topWidget->setDropMode(DropMode::FONDO);
                    topWidget->setBackgroundPath(path);
                });
                clickable->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                clickable->setFixedSize(100, 100);
                hLayout->addWidget(clickable);
            } else {
                auto* draggable = new DraggableLabel(fullPath);
                connect(draggable, &DraggableLabel::dragStarted, this, [this, fullPath]() {
                    if (topWidget->estaPincelActivo()) {
                        topWidget->setDropMode(DropMode::PINCEL_PISO);
                        topWidget->setPisoPath(fullPath);
                    } else {
                        topWidget->setDropMode(DropMode::OBJETO);
                    }
                });
                draggable->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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

TopWidget* MainWindow::gettopWidget() const {
    return this->topWidget;
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasImage() || event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

bool MainWindow::zonasValidadas(){
    const auto zonas = topWidget->getZonas();

    bool tieneCT = false;
    bool tieneTT = false;
    bool tieneBomba = false;

    for (const auto& zona : zonas) {
        if (zona.tipo == "inicio_ct") tieneCT = true;
        else if (zona.tipo == "inicio_tt") tieneTT = true;
        else if (zona.tipo == "zona_bombas") tieneBomba = true;
    }

    if (!tieneCT || !tieneTT || !tieneBomba) {
        QString mensaje = "Faltan zonas obligatorias:\n";
        if (!tieneCT) mensaje += "- Zona de inicio CT\n";
        if (!tieneTT) mensaje += "- Zona de inicio TT\n";
        if (!tieneBomba) mensaje += "- Zona de bombas\n";

        QMessageBox box(this);
        box.setWindowTitle("Guardado inválido");
        box.setText(mensaje);
        box.setIcon(QMessageBox::Warning);
        box.setStandardButtons(QMessageBox::Ok);

        box.setStyleSheet(R"(
            QMessageBox {
                background-color: #2e2e2e;
                color: #f0f0f0;
                font-size: 14px;
            }
            QPushButton {
                background-color: #444;
                color: #fff;
                padding: 6px 12px;
                border: 1px solid #666;
                border-radius: 4px;
            }
            QPushButton:hover {
                background-color: #555;
            }
        )");

        box.exec();
        return false;
    }

    return true;
}

void MainWindow::copiarMapaA(const QString& destinoDirectorio, const QString& origenYaml, const QString& origenJpg, const QString& nombreArchivo) {
    QDir dir(destinoDirectorio);
    dir.mkpath(".");

    QString destinoYaml = dir.filePath(nombreArchivo);
    QFile::remove(destinoYaml);
    if (!QFile::copy(origenYaml, destinoYaml)) {
        qWarning() << "Error al copiar YAML a" << destinoYaml;
    } else {
        qDebug() << "YAML copiado correctamente a:" << destinoYaml;
    }

    QString destinoThumb = destinoYaml;
    destinoThumb.replace(".yaml", ".jpg");
    QFile::remove(destinoThumb);
    if (!QFile::copy(origenJpg, destinoThumb)) {
        qWarning() << "Error al copiar miniatura a" << destinoThumb;
    } else {
        qDebug() << "Miniatura copiada correctamente a:" << destinoThumb;
    }
}

void MainWindow::guardarMapa() {
    if (!zonasValidadas()) return;

    QString rutaProyecto = QString(__FILE__);
    rutaProyecto.chop(QString("editor/editor_main_window.cpp").length());

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Guardar mapa",
        rutaProyecto + "editor/mapas",
        "Archivos YAML (*.yaml);;Todos los archivos (*)"
    );

    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".yaml", Qt::CaseInsensitive)) {
        fileName += ".yaml";
    }

    QString nombreArchivo = QFileInfo(fileName).fileName();

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo para escribir:" << file.errorString();
        QMessageBox::critical(this, "Error", "No se pudo guardar el mapa:\n" + file.errorString());
        return;
    }

    QTextStream out(&file);

    QString fondoPath = topWidget->getFondoPath();
    int posFondo = fondoPath.indexOf("gfx/");
    QString fondoRelativo = (posFondo != -1) ? fondoPath.mid(posFondo + 4) : fondoPath;
    out << "fondo: " << fondoRelativo << "\n";

    out << "ancho_max_mapa: " << topWidget->getMaxAncho() << "\n";
    out << "alto_max_mapa: " << topWidget->getMaxAlto() << "\n";
    out << "elementos:\n";

    for (const auto& e : topWidget->getElementos()) {
        QString path = e.path;
        int pos = path.indexOf("gfx/");
        QString rutaRelativa = (pos != -1) ? path.mid(pos + 4) : path;
        out << "  - imagen: " << rutaRelativa << "\n";
        out << "    x: " << int(e.posicion.x()) << "\n";
        out << "    y: " << int(e.posicion.y()) << "\n";
        out << "    tipo: " << e.tipo << "\n";
        out << "    ancho: " << e.ancho << "\n";
        out << "    alto: " << e.alto << "\n";
        out << "    prioridad: " << e.prioridad << "\n";
    }

    out << "zonas:\n";
    for (const auto& zona : topWidget->getZonas()) {
        out << "  - tipo: " << zona.tipo << "\n";
        out << "    x: " << int(zona.rect.x()) << "\n";
        out << "    y: " << int(zona.rect.y()) << "\n";
        out << "    ancho: " << int(zona.rect.width()) << "\n";
        out << "    alto: " << int(zona.rect.height()) << "\n";
        out << "    id: \"" << zona.id.toString(QUuid::WithoutBraces) << "\"\n";
    }

    file.close();

    // ============================
    // GUARDAR MINIATURA .JPG
    // ============================
    QString thumbPath = fileName;
    thumbPath.replace(".yaml", ".jpg");
    QImage thumbnail = topWidget->generarMiniatura();
    if (!thumbnail.save(thumbPath, "JPG")) {
        qWarning() << "No se pudo guardar la miniatura del mapa.";
    }

    // ============================
    // COPIAS ADICIONALES
    // ============================

    #ifdef INSTALADO
        copiarMapaA(RUTA_BASE_EDITOR, fileName, thumbPath, nombreArchivo);
        copiarMapaA(RUTA_SERVER_BASE, fileName, thumbPath, nombreArchivo);
    #endif

    copiarMapaA(rutaProyecto + "server_src/mapas_disponibles", fileName, thumbPath, nombreArchivo);

    QApplication::quit();
}

void MainWindow::cargarDesdeYAML(const QString& ruta) {
    YAML::Node root = YAML::LoadFile(ruta.toStdString());

    int ancho = root["ancho_max_mapa"] ? root["ancho_max_mapa"].as<int>() : 2048;
    int alto  = root["alto_max_mapa"] ? root["alto_max_mapa"].as<int>() : 2048;
    topWidget->setTamanioMapaDesdeYAML(ancho, alto);

    QString fondoRel = QString::fromStdString(root["fondo"].as<std::string>());
    QString fondoPath = RUTA_BASE_IMAGENES + fondoRel;
    topWidget->setDropMode(DropMode::FONDO);
    topWidget->setBackgroundPath(fondoPath);

    // ================================
    // CARGAR ELEMENTOS (ordenados por prioridad)
    // ================================
    const auto& elementos = root["elementos"];
    std::map<int, std::vector<YAML::Node>> elementosPorPrioridad;

    for (const auto& elemento : elementos) {
        int prioridad = 1;
        if (elemento["prioridad"])
            prioridad = elemento["prioridad"].as<int>();
        elementosPorPrioridad[prioridad].push_back(elemento);
    }

    for (const auto& [prioridad, lista] : elementosPorPrioridad) {
        for (const auto& elemento : lista) {
            QString imagenRel = QString::fromStdString(elemento["imagen"].as<std::string>());
            QString imagenPath = RUTA_BASE_IMAGENES + imagenRel;
            int x = elemento["x"].as<int>();
            int y = elemento["y"].as<int>();
            topWidget->agregarElemento(imagenPath, x, y);
        }
    }

    // ================================
    // CARGAR ZONAS
    // ================================
    const auto& zonas = root["zonas"];
    for (const auto& zona : zonas) {
        QString tipoZona = QString::fromStdString(zona["tipo"].as<std::string>());
        int x = zona["x"].as<int>();
        int y = zona["y"].as<int>();
        int ancho = zona["ancho"].as<int>();
        int alto = zona["alto"].as<int>();
        QRectF rect(x, y, ancho, alto);

        QUuid id;
        try {
            if (zona["id"]) {
                std::string idStr = zona["id"].as<std::string>();
                id = QUuid(QString::fromStdString(idStr));
            } else {
                id = QUuid::createUuid();
            }
        } catch (const std::exception& e) {
            qWarning() << "Error al convertir el id de zona a string:" << e.what() << "- Se generará un UUID nuevo.";
            id = QUuid::createUuid();
        }

        topWidget->agregarZona(rect, tipoZona, id);
    }
}

#include "personaje_popup.h"
#include <QPainter>
#include <QStyleOption>

PersonajePopup::PersonajePopup(QWidget* parent)
    : QDialog(parent)
{
    // Ventana sin borde y con fondo translúcido
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    setFixedSize(400, 280);

    QWidget* backgroundWidget = new QWidget(this);
    backgroundWidget->setStyleSheet(
        "background-color: rgba(60, 60, 60, 200);"
        "color: white;"
        "border-radius: 12px;"
    );
    backgroundWidget->setGeometry(this->rect());

    // Aquí asignamos el mainLayout al backgroundWidget para controlar todo
    mainLayout = new QVBoxLayout(backgroundWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel* label = new QLabel("Seleccione su equipo:", backgroundWidget);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 20px; font-weight: bold;");

    btnTerrorist = new QPushButton("Terrorist", backgroundWidget);
    btnCounterTerrorist = new QPushButton("Counter Terrorist", backgroundWidget);

    QString btnStyle =
        "QPushButton {"
        "  color: white;"
        "  background-color: rgba(255, 255, 255, 30);"
        "  border: 1px solid white;"
        "  padding: 8px 15px;"
        "  border-radius: 8px;"
        "  font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(255, 255, 255, 60);"
        "}";

    btnTerrorist->setStyleSheet(btnStyle);
    btnCounterTerrorist->setStyleSheet(btnStyle);

    mainLayout->addWidget(label);
    mainLayout->addWidget(btnTerrorist);
    mainLayout->addWidget(btnCounterTerrorist);

    skinsLayout = new QHBoxLayout();
    skinsWidget = new QWidget(this);
    skinsWidget->setLayout(skinsLayout);
    skinsWidget->setVisible(false);
    mainLayout->addWidget(skinsWidget);

    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(seleccionarSkin(int)));

    connect(btnTerrorist, &QPushButton::clicked, this, &PersonajePopup::seleccionarTerrorist);
    connect(btnCounterTerrorist, &QPushButton::clicked, this, &PersonajePopup::seleccionarCounterTerrorist);
}

void PersonajePopup::seleccionarTerrorist() {
    equipoSeleccionado = 0;
    mostrarOpcionesT();
}

void PersonajePopup::seleccionarCounterTerrorist() {
    equipoSeleccionado = 1;
    mostrarOpcionesCT();
}

void PersonajePopup::mostrarOpcionesCT() {
    // Ocultar botones de selección de equipo
    btnTerrorist->setVisible(false);
    btnCounterTerrorist->setVisible(false);

    // Mostrar instrucciones
    QLabel* label = new QLabel("Seleccione su skin counter terrorist:", this);
    mainLayout->insertWidget(1, label);  // Insertar debajo del título

    // Limpiar skins previas si hay
    QLayoutItem* item;
    while ((item = skinsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Rutas relativas o absolutas a las imágenes
    QString basePath = "client_src/gfx/player/";
    QStringList skinFiles = {
        "ct1.bmp",
        "ct2.bmp",
        "ct3.bmp",
        "ct4.bmp"
    };

    for (int i = 0; i < skinFiles.size(); ++i) {
        QPushButton* btnSkin = new QPushButton(this);

        QPixmap pixmap(basePath + skinFiles[i]);
        QIcon icon(pixmap);
        btnSkin->setIcon(icon);
        btnSkin->setIconSize(QSize(90, 90));
        btnSkin->setFixedSize(100, 100);

        skinsLayout->addWidget(btnSkin);

        connect(btnSkin, &QPushButton::clicked, this, [this, i]() {
            seleccionarSkin(i);
        });
    }

    skinsWidget->setVisible(true);
}


void PersonajePopup::mostrarOpcionesT() {
    // Ocultar botones de selección de equipo
    btnTerrorist->setVisible(false);
    btnCounterTerrorist->setVisible(false);

    // Mostrar instrucciones
    QLabel* label = new QLabel("Seleccione su skin terrorist:", this);
    mainLayout->insertWidget(1, label);  // Insertar debajo del título

    // Limpiar skins previas si hay
    QLayoutItem* item;
    while ((item = skinsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Rutas relativas o absolutas a las imágenes
    QString basePath = "client_src/gfx/player/"; 
    QStringList skinFiles = {
        "t1.bmp",
        "t2.bmp",
        "t3.bmp",
        "t4.bmp"
    };

    for (int i = 0; i < skinFiles.size(); ++i) {
        QPushButton* btnSkin = new QPushButton(this);

        QPixmap pixmap(basePath + skinFiles[i]);
        QIcon icon(pixmap);
        btnSkin->setIcon(icon);
        btnSkin->setIconSize(QSize(90, 90));
        btnSkin->setFixedSize(100, 100);

        skinsLayout->addWidget(btnSkin);

        connect(btnSkin, &QPushButton::clicked, this, [this, i]() {
            seleccionarSkin(i);
        });
    }

    skinsWidget->setVisible(true);
}

void PersonajePopup::seleccionarSkin(int skinIndex) {
    emit teamSeleccionado(equipoSeleccionado, skinIndex); // Enviar equipo y skin
    accept(); // cerrar diálogo
}

// Para que el fondo translúcido funcione bien
void PersonajePopup::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QDialog::paintEvent(event);
}

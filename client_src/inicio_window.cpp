#include "inicio_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QIntValidator>
#include <QPalette>
#include <QFont>

Inicio::Inicio(QWidget *parent) : QWidget(parent), ui(nullptr) {

    this->resize(900, 700); // Tamaño más grande para parecerse a la imagen

    // Fondo
    QPixmap fondo("client_src/gfx/backgrounds/lobby.jpg"); 
    if (!fondo.isNull()) {
        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(fondo.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
        this->setPalette(palette);
        this->setAutoFillBackground(true);
    }

    // Para centrar todo en la ventana, el layout principal será vertical y centrado
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setContentsMargins(100, 50, 100, 50);  // Márgenes para centrar y espacio

    // Fuente grande para los campos y labels
    QFont font;
    font.setPointSize(18);

    // Host
    auto *hostLabel = new QLabel("Host:", this);
    hostLabel->setFont(font);
    hostLabel->setAlignment(Qt::AlignCenter);

    auto *hostEdit = new QLineEdit(this);
    hostEdit->setFont(font);
    hostEdit->setFixedHeight(40);
    hostEdit->setAlignment(Qt::AlignCenter);

    // Puerto
    auto *portLabel = new QLabel("Port:", this);
    portLabel->setFont(font);
    portLabel->setAlignment(Qt::AlignCenter);

    auto *portEdit = new QLineEdit(this);
    portEdit->setFont(font);
    portEdit->setFixedHeight(40);
    portEdit->setValidator(new QIntValidator(1, 65535, this));
    portEdit->setAlignment(Qt::AlignCenter);

    // Usuario
    auto *userLabel = new QLabel("Username:", this);
    userLabel->setFont(font);
    userLabel->setAlignment(Qt::AlignCenter);

    auto *userEdit = new QLineEdit(this);
    userEdit->setFont(font);
    userEdit->setFixedHeight(40);
    userEdit->setAlignment(Qt::AlignCenter);

    // Botón conectar
    auto *connectButton = new QPushButton("Conectar", this);
    connectButton->setFont(font);
    connectButton->setFixedHeight(50);
    connectButton->setMinimumWidth(200);

    // Agrego widgets al layout con separación uniforme
    mainLayout->addWidget(hostLabel);
    mainLayout->addWidget(hostEdit);
    mainLayout->addSpacing(15);

    mainLayout->addWidget(portLabel);
    mainLayout->addWidget(portEdit);
    mainLayout->addSpacing(15);

    mainLayout->addWidget(userLabel);
    mainLayout->addWidget(userEdit);
    mainLayout->addSpacing(30);

    mainLayout->addWidget(connectButton);

    // Señal del botón
    connect(connectButton, &QPushButton::clicked, this, [this, hostEdit, portEdit, userEdit]() {
        QString host = hostEdit->text().trimmed();
        QString portStr = portEdit->text().trimmed();
        QString user = userEdit->text().trimmed();

        if (host.isEmpty() || portStr.isEmpty() || user.isEmpty()) {
            // Podés agregar un QMessageBox para avisar que falten campos
            return;
        }

        bool ok;
        quint16 port = portStr.toUShort(&ok);
        if (!ok) return;

        emit datosIngresados(host, port, user);
        this->close();
    });

    this->setStyleSheet(R"(
        QLabel {
            color: white;
        }
        QLineEdit {
            border: 2px solid white;
            border-radius: 8px;
            padding: 8px;
            background: rgba(255,255,255,0.2);
            color: white;
        }
        QPushButton {
            color: white;
            background-color: rgba(0, 0, 0, 120);
            border: 1px solid white;
            padding: 6px;
            border-radius: 10px;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 30);
        }
    
    )");

}

Inicio::~Inicio() = default;

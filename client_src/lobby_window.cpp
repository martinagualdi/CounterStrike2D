#include "lobby_window.h"
#include "mensaje_popup.h"
#include "personaje_popup.h"
#include <QVBoxLayout>
#include <QInputDialog>
#include <QPixmap>
#include <QStackedLayout>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include <QTimer>


LobbyWindow::LobbyWindow(ProtocoloCliente& protocolo, const std::string& username)
    : QDialog(nullptr), protocolo(protocolo) {

    this->resize(900, 700); // Tamaño más grande para parecerse a la imagen

    // Fondo
    QPixmap fondo("client_src/gfx/backgrounds/lobby.jpg"); 
    if (!fondo.isNull()) {
        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(fondo.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
        this->setPalette(palette);
        this->setAutoFillBackground(true);
    }

    // Layout principal (horizontal: izquierda botones, derecha vacío)
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // Layout izquierdo
    QVBoxLayout* leftLayout = new QVBoxLayout();

    // Etiqueta de bienvenida
    mensajeLabel = new QLabel(QString("Bienvenido a Counter Strike 2D, %1").arg(QString::fromStdString(username)), this);
    mensajeLabel->setStyleSheet("color: white; font-weight: bold; font-size: 30px;");

    // Botones más pequeños
    crearButton = new QPushButton("Crear partida", this);
    listarButton = new QPushButton("Listar partidas", this);
    unirseButton = new QPushButton("Unirse a partida", this);

    QString buttonStyle = R"(
        QPushButton {
            color: white;
            background-color: rgba(0, 0, 0, 120);
            border: 1px solid white;
            padding: 12px 24px;
            font-size: 18px;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 30);
        }
    )";

    crearButton->setStyleSheet(buttonStyle);
    listarButton->setStyleSheet(buttonStyle);
    unirseButton->setStyleSheet(buttonStyle);

    crearButton->setFixedSize(240, 50);
    listarButton->setFixedSize(240, 50);
    unirseButton->setFixedSize(240, 50);

    // Lista de partidas
    listaPartidas = new QListWidget(this);
    listaPartidas->setFixedWidth(240);
    listaPartidas->setStyleSheet("background-color: rgba(0, 0, 0, 100); color: white;");

    // Agregar widgets al layout izquierdo
    leftLayout->addSpacing(40);
    leftLayout->addWidget(mensajeLabel);
    leftLayout->addSpacing(20);
    leftLayout->addWidget(crearButton);
    leftLayout->addWidget(listarButton);
    leftLayout->addWidget(listaPartidas);
    leftLayout->addWidget(unirseButton);
    leftLayout->addStretch();

    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch(); 

    // Conexiones
    connect(crearButton, &QPushButton::clicked, this, &LobbyWindow::onCrearClicked);
    connect(listarButton, &QPushButton::clicked, this, &LobbyWindow::onListarClicked);
    connect(unirseButton, &QPushButton::clicked, this, &LobbyWindow::onUnirseClicked);

    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setSource(QUrl::fromLocalFile("client_src/sfx/gamestart.mp3"));
    audioOutput->setVolume(100); 
    mediaPlayer->play();
}


void LobbyWindow::onCrearClicked() {
    protocolo.enviar_crear_partida();

    PersonajePopup equipoDialog(this);
    connect(&equipoDialog, &PersonajePopup::skinSeleccionado, this, [this](int skinIndex) {
        qDebug() << "Skin seleccionada (crear):" << skinIndex;
        // protocolo.enviar_skin(skinIndex);
    });

    if (equipoDialog.exec() == QDialog::Accepted) {
        MensajePopup popup("Partida", "Partida creada con éxito.", this);
        popup.exec();
        emit partidaSeleccionada();
        fadeOutAudioAndClose();
    }
}    

void LobbyWindow::fadeOutAudioAndClose() {
    QTimer *fadeTimer = new QTimer(this);
    fadeTimer->setInterval(50);  // cada 50ms
    connect(fadeTimer, &QTimer::timeout, this, [this, fadeTimer]() {
        float vol = audioOutput->volume();
        if (vol > 0.01f) {
            audioOutput->setVolume(vol - 0.05f);  // bajamos el volumen
        } else {
            fadeTimer->stop();
            mediaPlayer->stop();
            accept();  // cerramos el diálogo
        }
    });
    fadeTimer->start();
}

void LobbyWindow::onListarClicked() {
    protocolo.enviar_listar_partida();
    QString lista = QString::fromStdString(protocolo.recibir_lista_partidas());
    listaPartidas->clear();
    for (const QString& linea : lista.split('\n', Qt::SkipEmptyParts)) {
        listaPartidas->addItem(linea);
    }
}

void LobbyWindow::onUnirseClicked() {
    QListWidgetItem* item = listaPartidas->currentItem();
    if (!item) {
        MensajePopup popup("Error", "Seleccione una partida.", this);
        popup.exec();
        return;
    }
    QString texto = item->text();
    QStringList partes = texto.split(" ");
    if (partes.isEmpty()) {
        MensajePopup popup("Error", "El texto del item está vacío.", this);
        popup.exec();
        return;
    }
    QString last = partes.last();
    bool ok;
    int id = last.toInt(&ok);
    if (!ok) {
        MensajePopup popup("Error", "ID no válido: " + last, this);
        popup.exec();
        return;
    }

    protocolo.enviar_unirse_partida(id);


    PersonajePopup equipoDialog(this);
    connect(&equipoDialog, &PersonajePopup::skinSeleccionado, this, [this](int skinIndex) {
        qDebug() << "Skin seleccionada (crear):" << skinIndex;
        // protocolo.enviar_skin(skinIndex);
    });


    if (equipoDialog.exec() == QDialog::Accepted) {
        MensajePopup popup("Partida", QString("Unido a la partida %1").arg(id), this);
        popup.exec();
        emit partidaSeleccionada();
        fadeOutAudioAndClose();
    }
}

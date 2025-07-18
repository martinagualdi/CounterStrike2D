#include "lobby_window.h"
#include "mensaje_popup.h"
#include "map_selection.h"
#include "../common_src/ruta_base.h"
#include <QVBoxLayout>
#include <QInputDialog>
#include <QPixmap>
#include <QStackedLayout>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include <QTimer>

LobbyWindow::LobbyWindow(ProtocoloCliente& protocolo, const std::string& username)
    : QDialog(nullptr), protocolo(protocolo), username(username) {

    this->resize(900, 700);

    QPixmap fondo(QString::fromStdString(RUTA_IMAGENES("lobby/lobby.jpg")));
    if (!fondo.isNull()) {
        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(fondo.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
        this->setPalette(palette);
        this->setAutoFillBackground(true);
    }

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    QVBoxLayout* leftLayout = new QVBoxLayout();

    mensajeLabel = new QLabel(QString("Bienvenido a Counter Strike 2D, %1").arg(QString::fromStdString(username)), this);
    mensajeLabel->setStyleSheet("color: white; font-weight: bold; font-size: 30px;");

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

    listaPartidas = new QListWidget(this);
    listaPartidas->setFixedWidth(240);
    listaPartidas->setStyleSheet("background-color: rgba(0, 0, 0, 100); color: white;");

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

    connect(crearButton, &QPushButton::clicked, this, &LobbyWindow::onCrearClicked);
    connect(listarButton, &QPushButton::clicked, this, &LobbyWindow::onListarClicked);
    connect(unirseButton, &QPushButton::clicked, this, &LobbyWindow::onUnirseClicked);

    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    std::string path_musica = RUTA_SONIDO("gamestart.mp3");
    mediaPlayer->setSource(QUrl::fromLocalFile(QString::fromStdString(path_musica)));
    audioOutput->setVolume(100); 
    mediaPlayer->play();
}

void LobbyWindow::closeEvent(QCloseEvent* event) {
    MensajePopup popup("Cerrando", "La partida se esta cerrando...", this);
    popup.exec();

    mediaPlayer->stop();
    event->accept();
}

void LobbyWindow::onCrearClicked() {
    protocolo.enviar_crear_partida(username);

    std::vector<std::pair<std::string, std::string>> mapas_disponibles = protocolo.recibir_lista_mapas();
    if (mapas_disponibles.empty()) {
        QMessageBox::warning(this, "Error", "No se recibieron mapas desde el servidor.");
        return;
    }

    QVector<QPair<QString, QString>> lista_mapas;
    for (const auto& m : mapas_disponibles) {
        QString nombre = QString::fromStdString(m.first);
        QString miniatura = QString::fromStdString(m.second);
        lista_mapas.append(qMakePair(nombre, miniatura));
    }

    MapSelectionDialog selector(lista_mapas, this);
    if (selector.exec() == QDialog::Accepted) {
        QString mapaSeleccionado = selector.getSelectedMap();
        protocolo.enviar_mensaje(mapaSeleccionado.toStdString());

        MensajePopup popup("Partida", "Partida creada con éxito.", this);
        popup.exec();
        emit partidaSeleccionada();
        fadeOutAudioAndClose();
    } else {
        protocolo.enviar_mensaje("cancelled");
    }
}

void LobbyWindow::fadeOutAudioAndClose() {
    QTimer *fadeTimer = new QTimer(this);
    fadeTimer->setInterval(50);
    connect(fadeTimer, &QTimer::timeout, this, [this, fadeTimer]() {
        float vol = audioOutput->volume();
        if (vol > 0.01f) {
            audioOutput->setVolume(vol - 0.05f);
        } else {
            fadeTimer->stop();
            mediaPlayer->stop();
            exitoLobby = true;
            accept(); 
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
    int id;
    QRegularExpression re("Id:\\s*(\\d+)");
    QRegularExpressionMatch match = re.match(texto);
    if (match.hasMatch()) {
        id = match.captured(1).toInt();
    } else {
        MensajePopup popup("Error", "No se pudo extraer el ID de la partida.", this);
        popup.exec();
        return;
    }

    protocolo.enviar_unirse_partida(id, username);
    std::string resultado = protocolo.recibir_mensaje();
    if (resultado == "failed") {
        MensajePopup popup("Error", "La partida ya está llena.", this);
        popup.exec();
        return;
    } else{
        MensajePopup popup("Partida", QString("Unido a la partida %1").arg(id), this);
        popup.exec();
        emit partidaSeleccionada();
        fadeOutAudioAndClose();
    }
}

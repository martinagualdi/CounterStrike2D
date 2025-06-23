#ifndef LOBBYWINDOW_H
#define LOBBYWINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QString>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QCloseEvent>
#include "client_protocol.h" 

class LobbyWindow : public QDialog{
    Q_OBJECT

public:
    explicit LobbyWindow(ProtocoloCliente& protocolo, const std::string& username);
    bool lobbySalioBien() const { return exitoLobby; }

signals:
    void partidaSeleccionada();

protected:
    void onCrearClicked();
    void onListarClicked();
    void onUnirseClicked();
    void fadeOutAudioAndClose();
    void closeEvent(QCloseEvent* event) override;

private:
    ProtocoloCliente& protocolo;
    std::string username;

    QPushButton* crearButton;
    QPushButton* listarButton;
    QPushButton* unirseButton;
    QListWidget* listaPartidas;
    QLabel* mensajeLabel;
    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;

    bool exitoLobby = false;
};

#endif // LOBBYWINDOW_H

#ifndef LOBBYWINDOW_H
#define LOBBYWINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QString>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "client_protocol.h" 

class LobbyWindow : public QDialog{
    Q_OBJECT

public:
    explicit LobbyWindow(ProtocoloCliente& protocolo, const std::string& username);

signals:
    void partidaSeleccionada();

private slots:
    void onCrearClicked();
    void onListarClicked();
    void onUnirseClicked();
    void fadeOutAudioAndClose();

private:
    ProtocoloCliente& protocolo;
    QPushButton* crearButton;
    QPushButton* listarButton;
    QPushButton* unirseButton;
    QListWidget* listaPartidas;
    QLabel* mensajeLabel;
    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;
};

#endif // LOBBYWINDOW_H

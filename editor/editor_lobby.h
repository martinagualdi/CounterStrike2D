#ifndef EDITOR_LOBBY_H
#define EDITOR_LOBBY_H

#include <QWidget>

class QPushButton;

class EditorLobby : public QWidget {
    Q_OBJECT

public:
    explicit EditorLobby(QWidget* parent = nullptr);

signals:
    void crearMapaSolicitado();
    void editarMapaSolicitado();
};

#endif // EDITOR_LOBBY_H
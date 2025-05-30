#include "editor_lobby.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>

EditorLobby::EditorLobby(QWidget* parent) : QWidget(parent) {
    this->setFixedSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(60, 40, 60, 40);

    QLabel* title = new QLabel("Bienvenido al editor de Mapas");
    QFont titleFont("Arial", 24, QFont::Bold);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);

    QPushButton* crearBtn = new QPushButton("Crear Mapa");
    QPushButton* editarBtn = new QPushButton("Editar Mapa");

    QFont buttonFont("Arial", 14);
    crearBtn->setFont(buttonFont);
    editarBtn->setFont(buttonFont);

    QString buttonStyle = R"(
        QPushButton {
            background-color: #3498db;
            color: white;
            border-radius: 10px;
            padding: 10px;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
    )";

    crearBtn->setStyleSheet(buttonStyle);
    editarBtn->setStyleSheet(buttonStyle);
    crearBtn->setMinimumHeight(50);
    editarBtn->setMinimumHeight(50);

    layout->addWidget(title);
    layout->addStretch();
    layout->addWidget(crearBtn);
    layout->addWidget(editarBtn);
    layout->addStretch();

    connect(crearBtn, &QPushButton::clicked, this, &EditorLobby::crearMapaSolicitado);
    connect(editarBtn, &QPushButton::clicked, this, &EditorLobby::editarMapaSolicitado);
}
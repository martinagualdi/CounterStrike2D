#ifndef MENSAJE_POPUP_H
#define MENSAJE_POPUP_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QStyleOption>

class MensajePopup : public QDialog {
    Q_OBJECT

public:
    explicit MensajePopup(const QString& titulo, const QString& mensaje, QWidget* parent = nullptr)
        : QDialog(parent) {
        // Hacer que la ventana tenga fondo translúcido
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);  // Sin borde de sistema
        setAttribute(Qt::WA_TranslucentBackground);
        setModal(true);
        setFixedSize(400, 200);

        // Widget de fondo estilizado
        QWidget* backgroundWidget = new QWidget(this);
        backgroundWidget->setStyleSheet(
            "background-color: rgba(60, 60, 60, 180);"  // Gris oscuro translúcido
            "color: white;"
            "font-size: 18px;"
            "border-radius: 10px;"
        );
        backgroundWidget->setGeometry(this->rect());

        QVBoxLayout* layout = new QVBoxLayout(backgroundWidget);

        QLabel* tituloLabel = new QLabel(titulo, this);
        tituloLabel->setStyleSheet("font-size: 22px; font-weight: bold;");
        tituloLabel->setAlignment(Qt::AlignCenter);

        QLabel* mensajeLabel = new QLabel(mensaje, this);
        mensajeLabel->setAlignment(Qt::AlignCenter);
        mensajeLabel->setWordWrap(true);

        QPushButton* okButton = new QPushButton("Aceptar", this);
        okButton->setStyleSheet(
            "QPushButton { color: white; background-color: rgba(255, 255, 255, 30); border: 1px solid white; padding: 10px 20px; }"
            "QPushButton:hover { background-color: rgba(255, 255, 255, 60); }"
        );
        okButton->setFixedWidth(120);
        connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

        layout->addWidget(tituloLabel);
        layout->addSpacing(10);
        layout->addWidget(mensajeLabel);
        layout->addStretch();
        layout->addWidget(okButton, 0, Qt::AlignCenter);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
        QDialog::paintEvent(event);
    }
};

#endif // MENSAJE_POPUP_H

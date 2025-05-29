#include "personaje_popup.h"
#include <QPainter>
#include <QStyleOption>

PersonajePopup::PersonajePopup(QWidget* parent)
    : QDialog(parent)
{
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

    mainLayout = new QVBoxLayout(backgroundWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel* label = new QLabel("Seleccione su skin:", backgroundWidget);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 20px; font-weight: bold;");
    mainLayout->addWidget(label);

    skinsLayout = new QHBoxLayout();
    skinsWidget = new QWidget(this);
    skinsWidget->setLayout(skinsLayout);
    mainLayout->addWidget(skinsWidget);

    mostrarSkins("client_src/gfx/player/", {"t1.bmp", "t2.bmp", "t3.bmp", "t4.bmp"});
}

void PersonajePopup::mostrarSkins(const QString& basePath, const QStringList& skinFiles) {
    QLayoutItem* item;
    while ((item = skinsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

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
    emit skinSeleccionado(skinIndex); 
    accept();
}


// Para que el fondo translúcido funcione bien
void PersonajePopup::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QDialog::paintEvent(event);
}

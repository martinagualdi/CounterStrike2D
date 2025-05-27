#ifndef PERSONAJE_WINDOW_H
#define PERSONAJE_WINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSignalMapper>

class PersonajePopup : public QDialog {
    Q_OBJECT

public:
    explicit PersonajePopup(QWidget* parent = nullptr);

signals:
    void teamSeleccionado(int team, int skinIndex = 0);  // 0 = Terrorist, 1 = Counter Terrorist

private slots:
    void seleccionarTerrorist();
    void seleccionarCounterTerrorist();
    void paintEvent(QPaintEvent* event) override;
    void seleccionarSkin(int skinIndex);

private:
    void mostrarOpcionesT();
    void mostrarOpcionesCT();

    QPushButton* btnTerrorist;
    QPushButton* btnCounterTerrorist;

    QWidget* skinsWidget;
    QHBoxLayout* skinsLayout;

    QVBoxLayout* mainLayout;

    QSignalMapper* signalMapper;

    int equipoSeleccionado = -1;
};

#endif // PERSONAJE_WINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QKeyEvent>
#include <QSet>
#include "Juego.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void actualizarFrame();

private:
    void renderizarEscena();
    void dibujarHUD(float scrollX);

    QGraphicsScene *escena;
    QGraphicsView *vista;
    QTimer *timer;
    Juego *juego;
    QSet<int> teclasPresionadas;
};

#endif // MAINWINDOW_H
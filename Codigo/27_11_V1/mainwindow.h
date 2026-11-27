#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStackedWidget>
#include <QMovie>
#include <QLabel>
#include <QBuffer>
#include <QTimer>
#include <QKeyEvent>
#include <QSet>
#include <QPixmap>
#include <QVector>
#include "Juego.h"
#include "GestorAudio.h"

class NivelRio;

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
    void alCambiarFrameVideo(int frameActual);
    void finalizarCinematica();

private:
    enum class EstadoPantalla {
        MenuPrincipal,
        MenuDificultad,
        Jugando
    };

    enum class TipoCinematica {
        Ninguna,
        Intro,
        Transicion,
        Victoria,
        GameOver
    };

    void cargarSprites();
    QVector<QPixmap> extraerFilaSprites(const QPixmap &hoja, double rx, double ry, double rw, double rh, int altoObjetivo);
    QPixmap recortarSinBordeBlanco(const QPixmap &hoja, double rx, double ry, double rw, double rh);
    QPixmap limpiarFondoCelda(const QPixmap &celda, int tamObjetivo);

    QString buscarRutaVideo(const QString &nombreArchivo) const;
    void reproducirCinematica(TipoCinematica tipo);
    void activarAudioNivelActual();
    void volverAlMenuPrincipal();

    void renderizarMenuPrincipal();
    void renderizarMenuDificultad();
    void renderizarEscena();
    void dibujarHUD(float scrollX);
    void renderizarEscenaRio(NivelRio *rio);

    QStackedWidget *pilaCentral;
    QGraphicsScene *escena;
    QGraphicsView *vista;
    QWidget *paginaVideo;
    QLabel *visorVideo;
    QMovie *pelicula;
    QBuffer *bufferVideo;
    QLabel *etiquetaSaltar;

    QTimer *timer;
    Juego *juego;
    GestorAudio *audio;
    QSet<int> teclasPresionadas;

    EstadoPantalla estadoPantalla;
    bool confirmandoSalidaMenu;
    bool mirandoDerecha;
    float tiempoAnimacion;
    bool spritesCargados;
    bool spritesNadoCargados;
    TipoCinematica cinematicaActual;
    int ultimoFrameVideo;
    bool rioViolentoActivo;

    // Sprites Nivel 1 y compartidos
    QPixmap fondoSelva;
    QVector<QPixmap> kapaxIdleDer, kapaxIdleIzq;
    QVector<QPixmap> kapaxCaminarDer, kapaxCaminarIzq;
    QPixmap kapaxSaltarDer, kapaxSaltarIzq;
    QPixmap kapaxAturdidoDer, kapaxAturdidoIzq;
    QPixmap monoIdleDer, monoIdleIzq;
    QPixmap monoLanzandoDer, monoLanzandoIzq;
    QPixmap spritePiedra;
    QPixmap spriteSuelo;
    QPixmap spriteMadera150;
    QPixmap spriteMadera160;
    QPixmap spriteLodo;
    QPixmap spriteVibora;
    QPixmap spriteGiftFortachon;
    QPixmap spriteGiftSaltarin;
    QPixmap spriteCorazon;

    // Sprites Nivel 2 (Río y natación)
    QVector<QPixmap> kapaxNadarArriba;
    QVector<QPixmap> kapaxNadarAbajo;
    QVector<QPixmap> kapaxNadarIzq;
    QVector<QPixmap> kapaxNadarDer;
    QVector<QPixmap> vegetacionOrilla;
    QPixmap spriteAguaRio;
    QPixmap spriteTroncoFlotante;
    QPixmap spriteVortice;
    QPixmap spriteJabalina;
    QPixmap tribuIdleDer, tribuIdleIzq;
    QPixmap tribuLanzandoDer, tribuLanzandoIzq;
};

#endif // MAINWINDOW_H
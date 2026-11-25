#include "mainwindow.h"
#include "NivelRibera.h"
#include "NivelRio.h"
#include "Lodazal.h"
#include "Resbaladero.h"
#include "Vibora.h"
#include "Tribu.h"
#include "TroncoFlotante.h"
#include "Vortice.h"
#include "GiftFortachon.h"
#include "GiftSaltarin.h"

#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    escena(new QGraphicsScene(this)),
    vista(new QGraphicsView(escena, this)),
    timer(new QTimer(this)),
    juego(new Juego(2)) {

    setWindowTitle("La Travesía de Kapax - Nivel 1: La Ribera del Magdalena");
    setFixedSize(960, 600);
    setFocusPolicy(Qt::StrongFocus);

    // Configurar la vista gráfica sin barras de desplazamiento
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setRenderHint(QPainter::Antialiasing);
    vista->setFocusPolicy(Qt::NoFocus);
    setCentralWidget(vista);

    // Iniciar la lógica del juego
    juego->iniciar();

    // Conectar el bucle principal a ~60 FPS (16 ms)
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarFrame);
    timer->start(16);
}

MainWindow::~MainWindow() {
    timer->stop();
    delete juego;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        return;
    }

    int tecla = event->key();
    teclasPresionadas.insert(tecla);

    Personaje *kapax = juego->getKapax();

    if (tecla == Qt::Key_P) {
        juego->pausar();
    } else if (tecla == Qt::Key_R) {
        juego->reiniciarNivel();
    } else if (!juego->estaPausado() && !juego->juegoTerminado() && kapax != nullptr) {
        if (tecla == Qt::Key_E) {
            kapax->consumirGift();
        } else if (tecla == Qt::Key_Space || tecla == Qt::Key_W || tecla == Qt::Key_Up) {
            if (juego->getNivelActual() == 1) {
                kapax->saltar();
            }
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        return;
    }
    teclasPresionadas.remove(event->key());
}

void MainWindow::actualizarFrame() {
    const float dt = 0.016f;
    Personaje *kapax = juego->getKapax();

    // Procesar movimiento según el nivel activo
    if (!juego->estaPausado() && !juego->juegoTerminado() && kapax != nullptr) {
        short dirX = 0;
        short dirY = 0;
        if (teclasPresionadas.contains(Qt::Key_A) || teclasPresionadas.contains(Qt::Key_Left)) {
            dirX -= 1;
        }
        if (teclasPresionadas.contains(Qt::Key_D) || teclasPresionadas.contains(Qt::Key_Right)) {
            dirX += 1;
        }
        kapax->moverHorizontal(dirX);
        if (dynamic_cast<NivelRio*>(juego->getNivel()) != nullptr) {
            if (teclasPresionadas.contains(Qt::Key_W) || teclasPresionadas.contains(Qt::Key_Up)) {
                dirY -= 1;
            }
            if (teclasPresionadas.contains(Qt::Key_S) || teclasPresionadas.contains(Qt::Key_Down)) {
                dirY += 1;
            }
            kapax->moverVertical(dirY);
        }
    }

    juego->actualizar(dt);
    renderizarEscena();
}

void MainWindow::renderizarEscena() {
    escena->clear();

    Nivel *nivel = juego->getNivel();
    NivelRibera *ribera = dynamic_cast<NivelRibera*>(nivel);
    NivelRio *rio = dynamic_cast<NivelRio*>(nivel);
    Personaje *kapax = juego->getKapax();

    float scrollX = (ribera != nullptr) ? ribera->getScrollX() : 0.0f;
    escena->setSceneRect(scrollX, 0.0f, 960.0f, 600.0f);
    if (rio != nullptr) {
        scrollX = 0.0f;
        setWindowTitle("La Travesía de Kapax - Nivel 2: Nadando el Magdalena");
        escena->setSceneRect(0.0f, 0.0f, 960.0f, 600.0f);
        escena->addRect(0.0f, 0.0f, 960.0f, 600.0f, QPen(Qt::NoPen), QBrush(QColor(17, 91, 126)));
        escena->addRect(0.0f, 0.0f, 62.0f, 600.0f, QPen(Qt::NoPen), QBrush(QColor(76, 91, 47)));
        escena->addRect(898.0f, 0.0f, 62.0f, 600.0f, QPen(Qt::NoPen), QBrush(QColor(76, 91, 47)));
        for (int y = 70; y < 600; y += 115) {
            escena->addLine(90.0f, static_cast<float>(y), 870.0f, static_cast<float>(y + 22),
                            QPen(QColor(75, 171, 190, 100), 3));
        }
    } else {
        setWindowTitle("La Travesía de Kapax - Nivel 1: La Ribera del Magdalena");
    }

    if (rio == nullptr) {
        escena->addRect(scrollX, 0.0f, 960.0f, 600.0f, QPen(Qt::NoPen), QBrush(QColor(135, 206, 235)));
        escena->addRect(scrollX, 380.0f, 960.0f, 220.0f, QPen(Qt::NoPen), QBrush(QColor(34, 95, 48)));
    }

    if (ribera != nullptr) {
        // 2. Meta: Orilla del río Magdalena
        float orillaX = ribera->getPosicionOrilla();
        escena->addRect(orillaX, 440.0f, 180.0f, 160.0f, QPen(Qt::NoPen), QBrush(QColor(30, 144, 255)));
        QGraphicsTextItem *metaTxt = escena->addText("RIO MAGDALENA");
        metaTxt->setDefaultTextColor(Qt::white);
        metaTxt->setFont(QFont("Arial", 10, QFont::Bold));
        metaTxt->setPos(orillaX + 10.0f, 415.0f);

        // 3. Plataformas (suelo y madera)
        for (Plataforma *plat : ribera->getPlataformas()) {
            if (plat != nullptr) {
                QColor colorPlat = (plat->getY() >= 490.0f) ? QColor(101, 67, 33) : QColor(139, 90, 43);
                escena->addRect(plat->getX(), plat->getY(), plat->getAncho(), plat->getAlto(),
                                QPen(Qt::black, 1), QBrush(colorPlat));
                // Franja de césped superior
                escena->addRect(plat->getX(), plat->getY(), plat->getAncho(), 6.0f,
                                QPen(Qt::NoPen), QBrush(QColor(50, 168, 82)));
            }
        }
    }

    if (nivel != nullptr) {
        // 4. Trampas (Lodazal, Resbaladero, Víbora)
        for (Trampa *t : nivel->getTrampas()) {
            if (t == nullptr || !t->estaActivo()) continue;

            if (dynamic_cast<Lodazal*>(t) != nullptr) {
                escena->addRect(t->getX(), t->getY(), t->getAncho(), t->getAlto(),
                                QPen(Qt::black), QBrush(QColor(78, 52, 46))); // Lodo marrón oscuro
            } else if (dynamic_cast<Resbaladero*>(t) != nullptr) {
                escena->addRect(t->getX(), t->getY(), t->getAncho(), t->getAlto(),
                                QPen(Qt::darkCyan), QBrush(QColor(129, 212, 250))); // Superficie resbaladiza
            } else if (dynamic_cast<Vibora*>(t) != nullptr) {
                escena->addEllipse(t->getX(), t->getY(), t->getAncho(), t->getAlto(),
                                   QPen(Qt::darkGreen, 2), QBrush(QColor(76, 175, 80))); // Víbora verde
            } else if (dynamic_cast<TroncoFlotante*>(t) != nullptr) {
                escena->addRect(t->getX(), t->getY(), t->getAncho(), t->getAlto(),
                                QPen(QColor(58, 38, 24), 2), QBrush(QColor(116, 78, 45)));
            } else if (dynamic_cast<Vortice*>(t) != nullptr) {
                escena->addEllipse(t->getX(), t->getY(), t->getAncho(), t->getAlto(),
                                   QPen(QColor(159, 231, 236), 3), QBrush(QColor(12, 63, 113)));
                escena->addEllipse(t->getX() + t->getAncho() * 0.25f,
                                   t->getY() + t->getAlto() * 0.25f,
                                   t->getAncho() * 0.5f, t->getAlto() * 0.5f,
                                   QPen(QColor(145, 216, 226), 2), QBrush(Qt::NoBrush));
            }
        }

        // 5. Gifts (Fortachón en rojo/dorado, Saltarín en cian)
        for (Gift *g : nivel->getGifts()) {
            if (g == nullptr || !g->estaActivo()) continue;

            if (dynamic_cast<GiftFortachon*>(g) != nullptr) {
                escena->addEllipse(g->getX(), g->getY(), g->getAncho(), g->getAlto(),
                                   QPen(Qt::yellow, 2), QBrush(QColor(229, 57, 53)));
            } else if (dynamic_cast<GiftSaltarin*>(g) != nullptr) {
                escena->addEllipse(g->getX(), g->getY(), g->getAncho(), g->getAlto(),
                                   QPen(Qt::white, 2), QBrush(QColor(0, 188, 212)));
            }
        }

        // 6. Enemigos (Monos) y sus proyectiles parabólicos
        for (Enemigo *e : nivel->getEnemigos()) {
            if (e == nullptr) continue;

            if (e->estaActivo()) {
                QColor colorMono;
                if (dynamic_cast<Tribu*>(e) != nullptr) {
                    colorMono = e->estaAnimandoAtaque() ? QColor(255, 201, 88) : QColor(191, 92, 48);
                } else {
                    escena->addRect(e->getX() - 15.0f, e->getY() + e->getAlto(), e->getAncho() + 30.0f, 10.0f,
                                    QPen(Qt::NoPen), QBrush(QColor(93, 64, 55)));
                    colorMono = e->estaAnimandoAtaque() ? QColor(180, 110, 60) : QColor(121, 85, 72);
                }
                escena->addRect(e->getX(), e->getY(), e->getAncho(), e->getAlto(),
                                QPen(Qt::black, 2), QBrush(colorMono));
            }

            // Proyectiles del agente inteligente
            for (Proyectil *p : e->getProyectiles()) {
                if (p != nullptr && p->estaActivo()) {
                    escena->addEllipse(p->getX(), p->getY(), p->getAncho(), p->getAlto(),
                                       QPen(Qt::black), QBrush(QColor(158, 158, 158)));
                }
            }
        }
    }

    // 7. Protagonista: Kapax
    if (kapax != nullptr && kapax->estaVivo()) {
        QColor colorKapax(230, 145, 56); // Tono base
        QPen bordeKapax(Qt::black, 2);

        if (kapax->isInvulnerable()) {
            bordeKapax = QPen(Qt::yellow, 4); // Aura de Fortachón
        } else if (kapax->isAturdido()) {
            colorKapax = QColor(156, 39, 176); // Indicador de Aturdido
        } else if (kapax->isPesado()) {
            colorKapax = QColor(110, 110, 110); // Indicador de Pesado
        }

        escena->addRect(kapax->getX(), kapax->getY(), kapax->getAncho(), kapax->getAlto(),
                        bordeKapax, QBrush(colorKapax));
    }

    // 8. Interfaz superior (HUD) y mensajes de estado
    dibujarHUD(scrollX);
}

void MainWindow::dibujarHUD(float scrollX) {
    Personaje *kapax = juego->getKapax();
    Nivel *nivel = juego->getNivel();

    // Barra semitransparente superior
    escena->addRect(scrollX, 0.0f, 960.0f, 44.0f, QPen(Qt::NoPen), QBrush(QColor(0, 0, 0, 150)));

    int vidas = (kapax != nullptr) ? kapax->getVidas() : 0;
    int salud = (kapax != nullptr) ? kapax->getSalud() : 0;
    int tiempo = (nivel != nullptr) ? static_cast<int>(nivel->getTiempoRestante()) : 0;
    QString giftTxt = (kapax != nullptr && kapax->tieneGiftDisponible()) ? "LISTO (Tecla E)" : "Ninguno";

    QString estadoTxt = "Normal";
    if (kapax != nullptr) {
        if (kapax->isInvulnerable()) estadoTxt = "FORTACHON";
        else if (kapax->isAturdido()) estadoTxt = "ATURDIDO";
        else if (kapax->isPesado()) estadoTxt = "PESADO";
    }

    QString info;
    if (dynamic_cast<NivelRio*>(nivel) != nullptr) {
        NivelRio *rio = static_cast<NivelRio*>(nivel);
        info = QString("Vidas: %1  |  Salud: %2/3  |  Tiempo: %3s  |  Corriente: %4  |  Gift: %5  |  Estado: %6  |  [WASD/Flechas: Nadar | E: Usar | P: Pausa | R: Reiniciar]")
                   .arg(vidas).arg(salud).arg(tiempo).arg(static_cast<int>(rio->getVelocidadCorriente()))
                   .arg(giftTxt).arg(estadoTxt);
    } else {
        info = QString("Vidas: %1  |  Salud: %2/3  |  Tiempo: %3s  |  Gift: %4  |  Estado: %5  |  [A/D: Mover | Espacio: Saltar | P: Pausa | R: Reiniciar]")
                   .arg(vidas).arg(salud).arg(tiempo).arg(giftTxt).arg(estadoTxt);
    }

    QGraphicsTextItem *hudTexto = escena->addText(info);
    hudTexto->setDefaultTextColor(Qt::white);
    hudTexto->setFont(QFont("Arial", 9, QFont::Bold));
    hudTexto->setPos(scrollX + 12.0f, 12.0f);

    // Mensajes centrales de Pausa, Victoria o Derrota
    if (juego->estaPausado() || juego->juegoTerminado()) {
        escena->addRect(scrollX + 260.0f, 220.0f, 440.0f, 120.0f,
                        QPen(Qt::white, 2), QBrush(QColor(0, 0, 0, 200)));

        QString titulo;
        if (juego->estaPausado()) {
            titulo = "PAUSA\nPresiona P para continuar";
        } else if (juego->hayVictoria()) {
            titulo = (juego->getNivelActual() == 2)
                         ? "¡JUEGO COMPLETADO!\nKapax sobrevivió al Magdalena (Presiona R para rejugar)"
                         : "¡NIVEL 1 COMPLETADO!\nLlegaste al Rio Magdalena";
        } else {
            titulo = "GAME OVER\nPresiona R para reintentar";
        }

        QGraphicsTextItem *msg = escena->addText(titulo);
        msg->setDefaultTextColor(Qt::yellow);
        msg->setFont(QFont("Arial", 14, QFont::Bold));
        msg->setPos(scrollX + 285.0f, 250.0f);
    }
}
#include "NivelRio.h"
#include "Cazador.h"
#include "TroncoFlotante.h"
#include "Remolino.h"
#include "Lodazal.h"
#include <cstdlib>
#include <cmath>

namespace {
const float ALTO_VENTANA = 600.0f;
const float MARGEN_HUD = 48.0f;          // el HUD ocupa los primeros 44 px
const float FACTOR_ARRASTRE_JUGADOR = 0.35f;
const float FACTOR_ORILLAS = 0.60f;      // las orillas (scroll) se mueven a 60 % de la corriente
const float LARGO_CICLO_ORILLA = 700.0f; // al salir por abajo, el cazador reaparece arriba
}

NivelRio::NivelRio(Personaje* p, short nivelDificultad, float anchoVista)
    : Nivel(p, nivelDificultad, 2, (nivelDificultad == 1 ? 50.0f : (nivelDificultad == 2 ? 70.0f : 90.0f))),
    velocidadCorriente(100.0f),
    incrementoCorriente(0.0f),
    scrollY(0.0f),
    anchoRio(680.0f),
    intervaloGeneracion(1.5f),
    tiempoDesdeGeneracion(0.0f),
    anchoVentana(anchoVista),
    limiteRioIzq(0.0f),
    limiteRioDer(0.0f),
    velocidadInicial(100.0f),
    velocidadMaxima(180.0f),
    vidasAnteriores(p != nullptr ? p->getVidas() : 3),
    tiempoMensajeVida(0.0f) {
}

NivelRio::~NivelRio() {
}

float NivelRio::aleatorio(float minimo, float maximo) const {
    float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return minimo + (maximo - minimo) * r;
}

int NivelRio::contarRemolinosActivos() const {
    int n = 0;
    for (Trampa* t : trampas) {
        if (t != nullptr && t->estaActivo() && dynamic_cast<Remolino*>(t) != nullptr) {
            ++n;
        }
    }
    return n;
}

void NivelRio::cargar() {
    limiteRioIzq = (anchoVentana - anchoRio) * 0.5f;
    limiteRioDer = limiteRioIzq + anchoRio;

    tiempoMensajeVida = 0.0f;
    scrollY = 0.0f;

    // Parámetros del río según la dificultad
    velocidadInicial = 100.0f * dificultad->getMultiplicadorCorriente();   // 100 / 140 / 180 px/s
    velocidadMaxima = velocidadInicial * 1.6f;
    velocidadCorriente = velocidadInicial;
    // Sube de forma lineal y llega al máximo justo al terminar el tiempo
    incrementoCorriente = (velocidadMaxima - velocidadInicial) / tiempoMeta;

    intervaloGeneracion = 1.1f / dificultad->getDensidadTrampas() + 0.4f;  // ~3.1 / 1.8 / 1.3 s
    tiempoDesdeGeneracion = intervaloGeneracion * 0.5f;

    // Jugador en el centro del río
    if (jugador != nullptr) {
        jugador->reaparecer((anchoVentana - jugador->getAncho()) * 0.5f, 430.0f);
        vidasAnteriores = jugador->getVidas();
    }

    // Cazadores de las orillas (los proyectiles usan tiro parabólico + agente que aprende)
    short nivDif = dificultad->obtenerNivel();
    short porIzquierda = (nivDif == 3) ? 2 : 1;   // fácil 1+1, normal 1+1, difícil 2+1
    short porDerecha = 1;

    const float anchoCazador = 42.0f;
    const float xIzq = limiteRioIzq - 40.0f - anchoCazador;   // 40 px de orilla antes del agua
    const float xDer = limiteRioDer + 40.0f;

    for (short i = 0; i < porIzquierda; ++i) {
        float y = -60.0f + i * (LARGO_CICLO_ORILLA / porIzquierda) + 80.0f;
        enemigos.push_back(new Cazador(xIzq, y, 0, *dificultad, 40.0f));
    }
    for (short i = 0; i < porDerecha; ++i) {
        float y = -60.0f + i * (LARGO_CICLO_ORILLA / porDerecha) + 330.0f;
        if (y > 640.0f) {
            y -= LARGO_CICLO_ORILLA;
        }
        enemigos.push_back(new Cazador(xDer, y, 1, *dificultad, 40.0f));
    }
}

void NivelRio::incrementarViolencia(float dt) {
    // A medida que avanza el tiempo el río se vuelve más rápido y violento
    velocidadCorriente += incrementoCorriente * dt;
    if (velocidadCorriente > velocidadMaxima) {
        velocidadCorriente = velocidadMaxima;
    }
}

void NivelRio::aplicarCorriente(float dt) {
    // Scroll vertical del paisaje
    scrollY += velocidadCorriente * FACTOR_ORILLAS * dt;

    // Troncos, remolinos y lodazales bajan arrastrados por el agua
    for (Trampa* t : trampas) {
        if (t == nullptr || !t->estaActivo()) {
            continue;
        }
        t->setY(t->getY() + velocidadCorriente * dt);
        if (t->getY() > ALTO_VENTANA + 40.0f) {
            t->desactivar();   // Nivel::limpiarInactivos() lo elimina
        }
    }

    // Los gifts arrojados al agua flotan con la corriente
    for (Gift* g : gifts) {
        if (g != nullptr && g->estaActivo() && !g->fueRecogido()) {
            g->setVelocidadCaida(velocidadCorriente);
        }
    }

    // Las orillas pasan a su ritmo: los cazadores avanzan con ellas y reaparecen arriba
    for (Enemigo* e : enemigos) {
        if (e == nullptr) {
            continue;
        }
        e->setY(e->getY() + velocidadCorriente * FACTOR_ORILLAS * dt);
        if (e->getY() > ALTO_VENTANA + 40.0f) {
            e->setY(e->getY() - LARGO_CICLO_ORILLA);
        }
    }

    // La corriente arrastra a Kapax río abajo (él nada contra ella con W / flecha arriba)
    if (jugador != nullptr && jugador->estaVivo()) {
        jugador->setY(jugador->getY() + velocidadCorriente * FACTOR_ARRASTRE_JUGADOR * dt);
    }
}

void NivelRio::generarObstaculos() {
    const float izq = limiteRioIzq + 10.0f;
    const float der = limiteRioDer - 10.0f;
    short nivDif = dificultad->obtenerNivel();

    // Con el paso del tiempo aparecen más remolinos
    float progreso = getProgreso();
    float probRemolino = 14.0f + 14.0f * progreso;
    float probLodo = 14.0f;
    if (contarRemolinosActivos() >= 2) {
        probRemolino = 0.0f;
    }

    float sorteo = aleatorio(0.0f, 100.0f);

    if (sorteo < probRemolino) {
        // Remolino: el radio de influencia depende de la dificultad (60 / 85 / 110 px)
        float radio = dificultad->getRadioVortice();
        float cx = aleatorio(limiteRioIzq + radio * 0.5f, limiteRioDer - radio * 0.5f);
        float intensidad = (nivDif == 1) ? 0.9f : (nivDif == 2 ? 1.1f : 1.3f);
        trampas.push_back(new Remolino(cx, -radio, radio, intensidad));
    } else if (sorteo < probRemolino + probLodo) {
        // Banco de lodo/algas: aplica la viscosidad y ralentiza a Kapax
        float ancho = aleatorio(110.0f, 170.0f);
        float x = aleatorio(izq, der - ancho);
        float coef = (nivDif == 1) ? 0.55f : (nivDif == 2 ? 0.65f : 0.72f);
        trampas.push_back(new Lodazal(x, -70.0f, ancho, 70.0f, coef));
    } else {
        // Tronco flotante
        float ancho1 = aleatorio(80.0f, 150.0f);
        float x1 = aleatorio(izq, der - ancho1);
        trampas.push_back(new TroncoFlotante(x1, -30.0f, ancho1, 28.0f));

        // En normal y difícil a veces baja un segundo tronco, dejando siempre un paso libre
        if (nivDif >= 2 && aleatorio(0.0f, 100.0f) < 35.0f) {
            float ancho2 = aleatorio(80.0f, 130.0f);
            const float pasoLibre = 190.0f;
            float x2;
            bool valido = false;
            if (x1 + ancho1 * 0.5f > (izq + der) * 0.5f) {
                float maxX = x1 - ancho2 - pasoLibre;
                if (maxX >= izq) {
                    x2 = aleatorio(izq, maxX);
                    valido = true;
                } else {
                    x2 = 0.0f;
                }
            } else {
                float minX = x1 + ancho1 + pasoLibre;
                if (minX <= der - ancho2) {
                    x2 = aleatorio(minX, der - ancho2);
                    valido = true;
                } else {
                    x2 = 0.0f;
                }
            }
            if (valido) {
                trampas.push_back(new TroncoFlotante(x2, -60.0f, ancho2, 28.0f));
            }
        }
    }
}

void NivelRio::despejarZonaReaparicion() {
    // Evita que Kapax reaparezca encima de una trampa mortal
    const float zx0 = 240.0f, zx1 = 720.0f, zy0 = 250.0f, zy1 = 640.0f;
    for (Trampa* t : trampas) {
        if (t == nullptr || !t->estaActivo()) {
            continue;
        }
        bool choca = (t->getX() < zx1) && (t->getX() + t->getAncho() > zx0) &&
                     (t->getY() < zy1) && (t->getY() + t->getAlto() > zy0);
        if (choca) {
            t->desactivar();
        }
    }
}

void NivelRio::actualizar(float dt) {
    if (completado || fallido || jugador == nullptr) {
        return;
    }

    // Aviso de "perdiste una vida": el nivel se congela unos segundos
    if (tiempoMensajeVida > 0.0f) {
        tiempoMensajeVida -= dt;
        if (tiempoMensajeVida < 0.0f) {
            tiempoMensajeVida = 0.0f;
        }
        return;
    }

    incrementarViolencia(dt);
    aplicarCorriente(dt);

    // Generación de obstáculos: cuanto más rápido el río, más seguido aparecen
    tiempoDesdeGeneracion += dt;
    float intervaloActual = intervaloGeneracion * (velocidadInicial / velocidadCorriente);
    if (intervaloActual < 0.5f) {
        intervaloActual = 0.5f;
    }
    if (tiempoDesdeGeneracion >= intervaloActual) {
        tiempoDesdeGeneracion = 0.0f;
        generarObstaculos();
    }

    // Movimiento del nadador (la entrada la fija la ventana con moverHorizontal/moverVertical)
    jugador->actualizar(dt);

    // Kapax no puede salir del río ni meterse bajo el HUD
    if (jugador->getX() < limiteRioIzq) {
        jugador->setX(limiteRioIzq);
    }
    if (jugador->getX() + jugador->getAncho() > limiteRioDer) {
        jugador->setX(limiteRioDer - jugador->getAncho());
    }
    if (jugador->getY() < MARGEN_HUD) {
        jugador->setY(MARGEN_HUD);
    }
    if (jugador->getY() + jugador->getAlto() > ALTO_VENTANA - 6.0f) {
        jugador->setY(ALTO_VENTANA - 6.0f - jugador->getAlto());
    }

    // Trampas, enemigos (agente inteligente), gifts, colisiones y temporizador
    Nivel::actualizar(dt);

    // Pérdida de vida: reaparece en el centro del río
    if (!completado && jugador->getVidas() < vidasAnteriores) {
        vidasAnteriores = jugador->getVidas();
        if (jugador->estaVivo()) {
            jugador->reaparecer((anchoVentana - jugador->getAncho()) * 0.5f, 430.0f);
            despejarZonaReaparicion();
            tiempoMensajeVida = 2.2f;
        }
    }
}

bool NivelRio::verificarVictoria() {
    if (jugador == nullptr || !jugador->estaVivo()) {
        return false;
    }
    // Se gana resistiendo el tiempo estipulado
    return tiempoTranscurrido >= tiempoMeta;
}

float NivelRio::getVelocidadCorriente() const { return velocidadCorriente; }
float NivelRio::getVelocidadInicial() const { return velocidadInicial; }
float NivelRio::getScrollY() const { return scrollY; }
float NivelRio::getAnchoRio() const { return anchoRio; }
float NivelRio::getLimiteRioIzq() const { return limiteRioIzq; }
float NivelRio::getLimiteRioDer() const { return limiteRioDer; }

float NivelRio::getProgreso() const {
    if (tiempoMeta <= 0.0f) {
        return 1.0f;
    }
    float p = tiempoTranscurrido / tiempoMeta;
    return (p > 1.0f) ? 1.0f : p;
}

bool NivelRio::mostrandoAvisoVida() const { return tiempoMensajeVida > 0.0f; }

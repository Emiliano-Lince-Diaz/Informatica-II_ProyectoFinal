#include "NivelRibera.h"
#include "Mono.h"
#include "Lodazal.h"
#include "Resbaladero.h"
#include "Vibora.h"
#include "GiftSaltarin.h"
#include "GiftFortachon.h"

NivelRibera::NivelRibera(Personaje* p, short nivelDificultad, float anchoVista)
    : Nivel(p, nivelDificultad, 1, (nivelDificultad == 1 ? 115.0f : (nivelDificultad == 2 ? 90.0f : 75.0f))),
    scrollX(0.0f),
    limiteScrollIzq(0.0f),
    limiteScrollDer(2400.0f - anchoVista),
    anchoMundo(2400.0f),
    posicionOrilla(2240.0f),
    anchoVentana(anchoVista),
    vidasAnteriores(p != nullptr ? p->getVidas() : 3),
    checkpointX(60.0f),
    checkpointY(435.0f),
    posicionCheckpoint(1140.0f),
    checkpointAlcanzado(false),
    tiempoMensajeVida(0.0f),
    tiempoMensajeCheckpoint(0.0f) {
}

NivelRibera::~NivelRibera() {
    for (Plataforma* plat : plataformas) {
        delete plat;
    }
    plataformas.clear();
}

void NivelRibera::cargar() {
    checkpointX = 60.0f;
    checkpointY = 435.0f;
    checkpointAlcanzado = false;
    tiempoMensajeVida = 0.0f;
    tiempoMensajeCheckpoint = 0.0f;

    if (jugador != nullptr) {
        jugador->reaparecer(checkpointX, checkpointY);
        vidasAnteriores = jugador->getVidas();
    }

    // 1. Plataformas de suelo (anchos múltiplos exactos de 40px para que los huecos coincidan al píxel)
    plataformas.push_back(new Plataforma(0.0f, 500.0f, 600.0f, 40.0f));    // Hueco 1: 600..700 (100px)
    plataformas.push_back(new Plataforma(700.0f, 500.0f, 520.0f, 40.0f));  // Checkpoint en x=1140 | Hueco 2: 1220..1320 (100px)
    plataformas.push_back(new Plataforma(1320.0f, 500.0f, 480.0f, 40.0f)); // Hueco 3: 1800..1880 (80px)
    plataformas.push_back(new Plataforma(1880.0f, 500.0f, 520.0f, 40.0f)); // Zona de la orilla

    // Plataformas elevadas de madera (~90px sobre el suelo, alcanzables con salto normal)
    plataformas.push_back(new Plataforma(310.0f, 410.0f, 150.0f, 22.0f));
    plataformas.push_back(new Plataforma(900.0f, 408.0f, 160.0f, 22.0f));
    plataformas.push_back(new Plataforma(1460.0f, 405.0f, 160.0f, 22.0f));
    plataformas.push_back(new Plataforma(1960.0f, 412.0f, 150.0f, 22.0f));

    short nivDif = (dificultad != nullptr) ? dificultad->obtenerNivel() : 2;

    // 2. Trampas y enemigos escalados según la dificultad
    if (nivDif == 1) {
        // FÁCIL
        trampas.push_back(new Lodazal(1430.0f, 490.0f, 100.0f, 14.0f, 0.55f));
        trampas.push_back(new Vibora(880.0f, 472.0f, 1.2f));

        enemigos.push_back(new Mono(740.0f, 210.0f, *dificultad));
        enemigos.push_back(new Mono(1750.0f, 205.0f, *dificultad));

        GiftFortachon* escudoExtra = new GiftFortachon(965.0f, 368.0f, 7.0f);
        escudoExtra->setVelocidadCaida(0.0f);
        gifts.push_back(escudoExtra);
    } else if (nivDif == 2) {
        // NORMAL
        trampas.push_back(new Lodazal(240.0f, 490.0f, 110.0f, 14.0f, 0.65f));
        trampas.push_back(new Lodazal(1420.0f, 490.0f, 115.0f, 14.0f, 0.68f));
        trampas.push_back(new Resbaladero(820.0f, 492.0f, 90.0f, 12.0f, 0.08f, 0.45f));
        trampas.push_back(new Vibora(520.0f, 472.0f, 1.5f));
        trampas.push_back(new Vibora(1680.0f, 472.0f, 1.5f));

        enemigos.push_back(new Mono(600.0f, 210.0f, *dificultad));
        enemigos.push_back(new Mono(1250.0f, 195.0f, *dificultad));
        enemigos.push_back(new Mono(1850.0f, 205.0f, *dificultad));
    } else {
        // DIFÍCIL
        trampas.push_back(new Lodazal(240.0f, 490.0f, 115.0f, 14.0f, 0.72f));
        trampas.push_back(new Lodazal(1420.0f, 490.0f, 120.0f, 14.0f, 0.75f));
        trampas.push_back(new Resbaladero(820.0f, 492.0f, 90.0f, 12.0f, 0.06f, 0.50f));
        trampas.push_back(new Resbaladero(1980.0f, 492.0f, 90.0f, 12.0f, 0.06f, 0.50f));
        trampas.push_back(new Vibora(520.0f, 472.0f, 1.6f));
        trampas.push_back(new Vibora(1020.0f, 472.0f, 1.6f));
        trampas.push_back(new Vibora(1680.0f, 472.0f, 1.6f));

        enemigos.push_back(new Mono(550.0f, 210.0f, *dificultad));
        enemigos.push_back(new Mono(1060.0f, 195.0f, *dificultad));
        enemigos.push_back(new Mono(1560.0f, 200.0f, *dificultad));
        enemigos.push_back(new Mono(2020.0f, 205.0f, *dificultad));
    }

    // 3. Gifts de doble salto (Saltarín) sobre troncos alcanzables
    gifts.push_back(new GiftSaltarin(370.0f, 370.0f, 1));
    gifts.push_back(new GiftSaltarin(1525.0f, 365.0f, 1));
}

void NivelRibera::actualizar(float dt) {
    if (completado || fallido || jugador == nullptr) {
        return;
    }

    if (tiempoMensajeVida > 0.0f) {
        tiempoMensajeVida -= dt;
        if (tiempoMensajeVida < 0.0f) {
            tiempoMensajeVida = 0.0f;
        }
        return;
    }

    if (tiempoMensajeCheckpoint > 0.0f) {
        tiempoMensajeCheckpoint -= dt;
        if (tiempoMensajeCheckpoint < 0.0f) {
            tiempoMensajeCheckpoint = 0.0f;
        }
    }

    jugador->aplicarGravedad(dt);
    jugador->actualizar(dt);

    if (jugador->getX() < 0.0f) {
        jugador->setX(0.0f);
    }
    if (jugador->getX() + jugador->getAncho() > anchoMundo) {
        jugador->setX(anchoMundo - jugador->getAncho());
    }

    resolverPlataformas(dt);

    if (!checkpointAlcanzado && jugador->getX() >= posicionCheckpoint) {
        checkpointAlcanzado = true;
        checkpointX = posicionCheckpoint;
        checkpointY = 435.0f;
        tiempoMensajeCheckpoint = 2.0f;
    }

    if (jugador->getY() > 650.0f) {
        if (jugador->isInvulnerable()) {
            jugador->recibirDano(1);
        }
        jugador->recibirDano(3);
    }

    Nivel::actualizar(dt);

    if (jugador->getVidas() < vidasAnteriores) {
        vidasAnteriores = jugador->getVidas();
        if (jugador->estaVivo()) {
            jugador->reaparecer(checkpointX, checkpointY);
            actualizarScroll();
            tiempoMensajeVida = 2.2f;
            return;
        }
    }

    if (!completado && tiempoTranscurrido >= tiempoMeta) {
        fallido = true;
    }

    actualizarScroll();
}

bool NivelRibera::verificarVictoria() {
    if (jugador == nullptr || !jugador->estaVivo()) {
        return false;
    }
    return (jugador->getX() >= posicionOrilla) && (tiempoTranscurrido <= tiempoMeta);
}

void NivelRibera::actualizarScroll() {
    if (jugador == nullptr) {
        return;
    }

    float deseado = jugador->getX() - (anchoVentana * 0.4f);
    if (deseado < limiteScrollIzq) {
        deseado = limiteScrollIzq;
    }
    if (deseado > limiteScrollDer) {
        deseado = limiteScrollDer;
    }
    scrollX = deseado;
}

void NivelRibera::resolverPlataformas(float dt) {
    if (jugador == nullptr) {
        return;
    }

    jugador->setEnElSuelo(false);

    for (Plataforma* plat : plataformas) {
        if (plat != nullptr && plat->colisionaCon(*jugador)) {
            float piesPrevios = (jugador->getY() + jugador->getAlto()) - (jugador->getVy() * dt);
            if (jugador->getVy() >= 0.0f && piesPrevios <= plat->superficieY() + 20.0f) {
                jugador->aterrizar(plat->superficieY());
            }
        }
    }
}

float NivelRibera::getScrollX() const { return scrollX; }
float NivelRibera::getAnchoMundo() const { return anchoMundo; }
float NivelRibera::getPosicionOrilla() const { return posicionOrilla; }
float NivelRibera::getPosicionCheckpoint() const { return posicionCheckpoint; }
bool NivelRibera::isCheckpointAlcanzado() const { return checkpointAlcanzado; }
bool NivelRibera::mostrandoAvisoVida() const { return tiempoMensajeVida > 0.0f; }
bool NivelRibera::mostrandoAvisoCheckpoint() const { return tiempoMensajeCheckpoint > 0.0f; }
const std::vector<Plataforma*>& NivelRibera::getPlataformas() const { return plataformas; }
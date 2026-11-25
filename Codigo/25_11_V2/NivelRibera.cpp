#include "NivelRibera.h"
#include "Mono.h"
#include "Lodazal.h"
#include "Resbaladero.h"
#include "Vibora.h"
#include "GiftSaltarin.h"

NivelRibera::NivelRibera(Personaje* p, short nivelDificultad, float anchoVista)
    : Nivel(p, nivelDificultad, 1, 90.0f), // Nivel 1 con límite de 90 segundos
    scrollX(0.0f),
    limiteScrollIzq(0.0f),
    limiteScrollDer(2400.0f - anchoVista),
    anchoMundo(2400.0f),
    posicionOrilla(2240.0f),
    anchoVentana(anchoVista) {
}

NivelRibera::~NivelRibera() {
    for (Plataforma* plat : plataformas) {
        delete plat;
    }
    plataformas.clear();
}

void NivelRibera::cargar() {
    // Posicionar a Kapax al inicio de la selva ribereña
    if (jugador != nullptr) {
        jugador->reaparecer(60.0f, 400.0f);
    }

    // 1. Plataformas de suelo y elevadas (con huecos para saltar)
    plataformas.push_back(new Plataforma(0.0f, 500.0f, 620.0f, 40.0f));
    plataformas.push_back(new Plataforma(700.0f, 500.0f, 520.0f, 40.0f));
    plataformas.push_back(new Plataforma(1310.0f, 500.0f, 480.0f, 40.0f));
    plataformas.push_back(new Plataforma(1870.0f, 500.0f, 530.0f, 40.0f)); // Zona de la orilla

    // Plataformas elevadas de madera
    plataformas.push_back(new Plataforma(340.0f, 380.0f, 150.0f, 22.0f));
    plataformas.push_back(new Plataforma(920.0f, 370.0f, 160.0f, 22.0f));
    plataformas.push_back(new Plataforma(1480.0f, 350.0f, 160.0f, 22.0f));
    plataformas.push_back(new Plataforma(1960.0f, 380.0f, 150.0f, 22.0f));

    // 2. Trampas del Nivel 1: Lodazales (Viscosidad), Resbaladeros (Fricción) y Víboras
    trampas.push_back(new Lodazal(260.0f, 486.0f, 110.0f, 16.0f, 0.65f));
    trampas.push_back(new Lodazal(1420.0f, 486.0f, 120.0f, 16.0f, 0.70f));

    trampas.push_back(new Resbaladero(820.0f, 488.0f, 85.0f, 14.0f, 0.08f, 0.45f));
    trampas.push_back(new Resbaladero(1980.0f, 488.0f, 85.0f, 14.0f, 0.08f, 0.45f));

    trampas.push_back(new Vibora(520.0f, 472.0f, 1.5f));
    trampas.push_back(new Vibora(1120.0f, 472.0f, 1.5f));
    trampas.push_back(new Vibora(1680.0f, 472.0f, 1.5f));

    // 3. Gifts estáticos de doble salto (Saltarín) en posiciones estratégicas
    gifts.push_back(new GiftSaltarin(395.0f, 340.0f, 1));
    gifts.push_back(new GiftSaltarin(1540.0f, 310.0f, 1));

    // 4. Enemigos (Monos lanzadores con Agente Inteligente en ramas altas)
    enemigos.push_back(new Mono(600.0f, 210.0f, *dificultad));
    enemigos.push_back(new Mono(1250.0f, 195.0f, *dificultad));
    enemigos.push_back(new Mono(1850.0f, 205.0f, *dificultad));
}

void NivelRibera::actualizar(float dt) {
    if (completado || fallido || jugador == nullptr) {
        return;
    }

    // Físicas de plataformero sobre Kapax
    jugador->aplicarGravedad(dt);
    jugador->actualizar(dt);

    // Mantener a Kapax dentro de los límites horizontales del mundo
    if (jugador->getX() < 0.0f) {
        jugador->setX(0.0f);
    }
    if (jugador->getX() + jugador->getAncho() > anchoMundo) {
        jugador->setX(anchoMundo - jugador->getAncho());
    }

    resolverPlataformas(dt);

    // Si cae por un hueco entre plataformas, pierde una vida y reaparece
    if (jugador->getY() > 650.0f) {
        jugador->recibirDano(3);
        if (jugador->estaVivo()) {
            float respawnX = (scrollX > 60.0f) ? (scrollX + 60.0f) : 60.0f;
            jugador->reaparecer(respawnX, 340.0f);
        }
    }

    // Lógica común: temporizador, enemigos, proyectiles, trampas, gifts y colisiones
    Nivel::actualizar(dt);

    // En el Nivel 1 también se pierde si se agota el tiempo estipulado antes de llegar al río
    if (!completado && tiempoTranscurrido >= tiempoMeta) {
        fallido = true;
    }

    actualizarScroll();
}

bool NivelRibera::verificarVictoria() {
    if (jugador == nullptr || !jugador->estaVivo()) {
        return false;
    }
    // Victoria al llegar a la orilla del río Magdalena antes de que se acabe el tiempo
    return (jugador->getX() >= posicionOrilla) && (tiempoTranscurrido <= tiempoMeta);
}

void NivelRibera::actualizarScroll() {
    if (jugador == nullptr) {
        return;
    }

    // Centrar la cámara en Kapax respetando los límites del scroll lateral limitado
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
            // Aterrizar solo si está cayendo (vy >= 0) y venía por encima de la superficie
            float piesPrevios = (jugador->getY() + jugador->getAlto()) - (jugador->getVy() * dt);
            if (jugador->getVy() >= 0.0f && piesPrevios <= plat->superficieY() + 18.0f) {
                jugador->aterrizar(plat->superficieY());
            }
        }
    }
}

float NivelRibera::getScrollX() const { return scrollX; }
float NivelRibera::getAnchoMundo() const { return anchoMundo; }
float NivelRibera::getPosicionOrilla() const { return posicionOrilla; }
const std::vector<Plataforma*>& NivelRibera::getPlataformas() const { return plataformas; }
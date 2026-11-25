#include "NivelRio.h"
#include "GiftFortachon.h"
#include "GiftSaltarin.h"
#include "Lodazal.h"
#include "TroncoFlotante.h"
#include "Tribu.h"
#include "Vortice.h"

NivelRio::NivelRio(Personaje* p, short nivelDificultad)
    : Nivel(p, nivelDificultad, 2, 60.0f),
      velocidadCorriente(38.0f * dificultad->getMultiplicadorCorriente()),
      scrollY(0.0f) {
}

void NivelRio::cargar() {
    if (jugador != nullptr) {
        jugador->reaparecer(460.0f, 270.0f);
    }

    trampas.push_back(new TroncoFlotante(140.0f, 45.0f, velocidadCorriente));
    trampas.push_back(new TroncoFlotante(565.0f, 195.0f, velocidadCorriente * 0.85f));
    trampas.push_back(new TroncoFlotante(320.0f, 385.0f, velocidadCorriente * 1.1f));
    trampas.push_back(new TroncoFlotante(720.0f, 520.0f, velocidadCorriente * 0.9f));

    trampas.push_back(new Vortice(285.0f, 210.0f, dificultad->getRadioVortice(), velocidadCorriente * 0.22f));
    trampas.push_back(new Vortice(650.0f, 85.0f, dificultad->getRadioVortice(), velocidadCorriente * 0.22f));
    trampas.push_back(new Vortice(455.0f, 445.0f, dificultad->getRadioVortice(), velocidadCorriente * 0.22f));

    trampas.push_back(new Lodazal(490.0f, 130.0f, 115.0f, 90.0f, 0.55f));
    trampas.push_back(new Lodazal(155.0f, 365.0f, 120.0f, 90.0f, 0.60f));

    gifts.push_back(new GiftSaltarin(425.0f, 105.0f, 1));
    enemigos.push_back(new Tribu(8.0f, 275.0f, *dificultad));
    enemigos.push_back(new Tribu(916.0f, 275.0f, *dificultad));
}

void NivelRio::actualizar(float dt) {
    if (completado || fallido || jugador == nullptr) {
        return;
    }

    velocidadCorriente += 0.9f * dt;
    jugador->actualizar(dt);
    jugador->mover(0.0f, velocidadCorriente * dt);

    const float margen = 8.0f;
    if (jugador->getX() < 65.0f) {
        jugador->setX(65.0f);
    } else if (jugador->getX() + jugador->getAncho() > 895.0f) {
        jugador->setX(895.0f - jugador->getAncho());
    }
    if (jugador->getY() < margen) {
        jugador->setY(margen);
    } else if (jugador->getY() + jugador->getAlto() > 592.0f) {
        jugador->setY(592.0f - jugador->getAlto());
    }

    for (Trampa* trampa : trampas) {
        if (auto* tronco = dynamic_cast<TroncoFlotante*>(trampa)) {
            tronco->setVelocidadCorriente(velocidadCorriente);
        } else if (auto* vortice = dynamic_cast<Vortice*>(trampa)) {
            vortice->setVelocidadCorriente(velocidadCorriente * 0.22f);
        }
    }

    Nivel::actualizar(dt);
    for (Gift* gift : gifts) {
        if (gift == nullptr || !gift->estaActivo()) {
            continue;
        }

        if (gift->getX() < 65.0f) {
            gift->setX(90.0f);
        } else if (gift->getX() + gift->getAncho() > 895.0f) {
            gift->setX(850.0f);
        }
    }

    if (!completado && tiempoTranscurrido >= tiempoMeta) {
        completado = true;
    }
}

bool NivelRio::verificarVictoria() {
    return jugador != nullptr && jugador->estaVivo() && tiempoTranscurrido >= tiempoMeta;
}

float NivelRio::getVelocidadCorriente() const {
    return velocidadCorriente;
}

float NivelRio::getScrollY() const {
    return scrollY;
}

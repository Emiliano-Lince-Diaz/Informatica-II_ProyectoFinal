#include "Nivel.h"

Nivel::Nivel(Personaje* p, short nivelDificultad, short numNivel, float tMeta)
    : numero(numNivel),
    tiempoMeta(tMeta),
    tiempoTranscurrido(0.0f),
    completado(false),
    fallido(false),
    jugador(p),
    dificultad(new Dificultad(nivelDificultad)),
    tiempoDesdeChequeoGift(0.0f),
    cooldownContactoEnemigo(0.0f) {
}

Nivel::~Nivel() {
    for (Enemigo* e : enemigos) {
        delete e;
    }
    enemigos.clear();

    for (Trampa* t : trampas) {
        delete t;
    }
    trampas.clear();

    for (Gift* g : gifts) {
        delete g;
    }
    gifts.clear();

    if (dificultad != nullptr) {
        delete dificultad;
        dificultad = nullptr;
    }
}

void Nivel::actualizar(float dt) {
    if (completado || fallido || jugador == nullptr) {
        return;
    }

    actualizarTemporizador(dt);

    if (cooldownContactoEnemigo > 0.0f) {
        cooldownContactoEnemigo -= dt;
        if (cooldownContactoEnemigo < 0.0f) {
            cooldownContactoEnemigo = 0.0f;
        }
    }

    tiempoDesdeChequeoGift += dt;
    bool intentarSoltarGift = false;
    if (tiempoDesdeChequeoGift >= 6.0f) {
        tiempoDesdeChequeoGift = 0.0f;
        intentarSoltarGift = true;
    }

    for (Enemigo* e : enemigos) {
        if (e != nullptr) {
            e->actualizar(dt, *jugador);

            if (intentarSoltarGift && e->estaActivo()) {
                Gift* nuevoGift = e->soltarGift();
                if (nuevoGift != nullptr) {
                    gifts.push_back(nuevoGift);
                }
            }
        }
    }

    for (Trampa* t : trampas) {
        if (t != nullptr && t->estaActivo()) {
            t->actualizar(dt);
        }
    }

    for (Gift* g : gifts) {
        if (g != nullptr && g->estaActivo()) {
            g->actualizar(dt);
            if (g->getY() > 800.0f) {
                g->desactivar();
            }
        }
    }

    resolverColisiones(dt);
    limpiarInactivos();

    if (verificarVictoria()) {
        completado = true;
    } else if (verificarDerrota()) {
        fallido = true;
    }
}

void Nivel::actualizarTemporizador(float dt) {
    tiempoTranscurrido += dt;
}

void Nivel::resolverColisiones(float dt) {
    if (jugador == nullptr || !jugador->estaVivo()) {
        return;
    }

    // Colisiones con trampas
    for (Trampa* t : trampas) {
        if (t != nullptr && t->estaActivo() && t->colisionaCon(*jugador)) {
            t->aplicarEfecto(*jugador, dt);
        }
    }

    // Colisión directa al tocar un enemigo (ej. saltar y chocar contra un mono)
    if (cooldownContactoEnemigo <= 0.0f) {
        for (Enemigo* e : enemigos) {
            if (e != nullptr && e->estaActivo() && e->colisionaCon(*jugador)) {
                jugador->recibirDano(1);
                jugador->setVy(-240.0f); // Rebote suave para separarlo del enemigo
                cooldownContactoEnemigo = 1.0f;
                break;
            }
        }
    }

    // Colisiones con gifts
    for (Gift* g : gifts) {
        if (g != nullptr && g->estaActivo() && g->colisionaCon(*jugador)) {
            jugador->recogerGift(g);
        }
    }
}

bool Nivel::verificarDerrota() {
    if (jugador == nullptr || !jugador->estaVivo()) {
        return true;
    }
    return false;
}

void Nivel::limpiarInactivos() {
    for (auto it = trampas.begin(); it != trampas.end(); ) {
        if (*it != nullptr && !(*it)->estaActivo()) {
            delete *it;
            it = trampas.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = gifts.begin(); it != gifts.end(); ) {
        if (*it != nullptr && !(*it)->estaActivo() && !(*it)->fueRecogido()) {
            delete *it;
            it = gifts.erase(it);
        } else {
            ++it;
        }
    }
}

short Nivel::getNumero() const { return numero; }
float Nivel::getTiempoMeta() const { return tiempoMeta; }
float Nivel::getTiempoTranscurrido() const { return tiempoTranscurrido; }

float Nivel::getTiempoRestante() const {
    float restante = tiempoMeta - tiempoTranscurrido;
    return (restante > 0.0f) ? restante : 0.0f;
}

bool Nivel::estaCompletado() const { return completado; }
bool Nivel::estaFallido() const { return fallido; }
Personaje* Nivel::getJugador() const { return jugador; }
const std::vector<Enemigo*>& Nivel::getEnemigos() const { return enemigos; }
const std::vector<Trampa*>& Nivel::getTrampas() const { return trampas; }
const std::vector<Gift*>& Nivel::getGifts() const { return gifts; }
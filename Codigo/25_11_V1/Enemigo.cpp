#include "Enemigo.h"
#include "GiftFortachon.h"
#include <cstdlib>

Enemigo::Enemigo(float x, float y, const Dificultad& d, float ancho, float alto, float rango, short tipoProj)
    : Entidad(x, y, ancho, alto),
    cadencia(d.getCadenciaLanzamiento()),
    tiempoDesdeDisparo(0.0f),
    rangoAlcance(rango),
    probabilidadGift(0.25f),
    tipoProyectil(tipoProj),
    agente(new AgenteLanzador(d, rango)) {
}

Enemigo::~Enemigo() {
    if (agente != nullptr) {
        delete agente;
        agente = nullptr;
    }

    for (Proyectil* p : proyectiles) {
        delete p;
    }
    proyectiles.clear();
}

void Enemigo::actualizar(float dt) {
    if (agente != nullptr) {
        agente->actualizar(dt);
    }
}

void Enemigo::actualizar(float dt, Personaje& p) {
    // Actualizar animación del agente
    actualizar(dt);

    // Si el enemigo sigue activo, controlar cadencia e intentar lanzar
    if (activo) {
        tiempoDesdeDisparo += dt;
        if (tiempoDesdeDisparo >= cadencia) {
            lanzar(p);
        }
    }

    // Actualizar, evaluar impactos/aprendizaje y limpiar proyectiles terminados
    evaluarProyectiles(p, dt);
    limpiarProyectiles();
}

void Enemigo::lanzar(const Personaje& p) {
    if (agente == nullptr || !activo) {
        return;
    }

    Proyectil* nuevoProyectil = agente->ejecutarCiclo(*this, p, tipoProyectil);
    if (nuevoProyectil != nullptr) {
        proyectiles.push_back(nuevoProyectil);
        tiempoDesdeDisparo = 0.0f;
    }
}

Gift* Enemigo::soltarGift() {
    float aleatorio = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    if (aleatorio <= probabilidadGift) {
        return new GiftFortachon(x + ancho * 0.25f, y + alto * 0.5f, 6.0f);
    }
    return nullptr;
}

void Enemigo::evaluarProyectiles(Personaje& p, float dt, float minX, float maxX, float maxY) {
    for (Proyectil* proj : proyectiles) {
        if (proj == nullptr) {
            continue;
        }

        if (proj->estaActivo()) {
            proj->actualizar(dt);
            proj->evaluarImpacto(p);

            if (proj->estaActivo() && proj->salioDeLimites(minX, maxX, maxY)) {
                proj->desactivar();
            }
        }

        // Si el proyectil impactó o salió de límites, alimentar la fase de Aprendizaje
        if (proj->termino() && !proj->fueEvaluado() && agente != nullptr) {
            agente->evaluarProyectil(proj);
        }
    }
}

void Enemigo::limpiarProyectiles() {
    for (auto it = proyectiles.begin(); it != proyectiles.end(); ) {
        Proyectil* proj = *it;
        if (proj != nullptr && proj->termino() && proj->fueEvaluado()) {
            delete proj;
            it = proyectiles.erase(it);
        } else {
            ++it;
        }
    }
}

const std::vector<Proyectil*>& Enemigo::getProyectiles() const {
    return proyectiles;
}

bool Enemigo::estaAnimandoAtaque() const {
    return (agente != nullptr) ? agente->estaAnimandoAtaque() : false;
}

short Enemigo::getTipoProyectil() const {
    return tipoProyectil;
}
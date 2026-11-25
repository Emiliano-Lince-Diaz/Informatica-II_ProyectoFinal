#include "Aprendizaje.h"

Aprendizaje::Aprendizaje(float tasa, std::size_t maxHist)
    : puntajes(5, std::vector<float>(4, 0.0f)), // 5 contextos (0..4), 4 estrategias (0..3)
    factorAnticipacion(0.6f),
    tasaAprendizaje(tasa),
    maxHistorial(maxHist) {
}

Aprendizaje::~Aprendizaje() {
}

float Aprendizaje::calcularRecompensa(short resultado, float distanciaMinima) const {
    if (resultado == 1) {
        return 10.0f;  // Acertó al jugador
    }
    if (resultado == 2) {
        // Pasó cerca: recompensa positiva proporcional a la cercanía
        float bonus = (90.0f - distanciaMinima) / 30.0f;
        return (bonus > 0.5f) ? bonus : 0.5f;
    }
    return -2.0f;      // Falló lejos
}

void Aprendizaje::aprender(short contexto, short estrategia, short resultado, float distanciaMinima, float errorX) {
    if (contexto < 0 || contexto >= static_cast<short>(puntajes.size())) {
        return;
    }
    if (estrategia < 0 || estrategia >= static_cast<short>(puntajes[contexto].size())) {
        return;
    }

    // 1. Calcular recompensa
    float recompensa = calcularRecompensa(resultado, distanciaMinima);

    // 2. Actualizar el puntaje de la pareja (contexto, estrategia) con promedio móvil exponencial
    float puntajeActual = puntajes[contexto][estrategia];
    puntajes[contexto][estrategia] = puntajeActual + tasaAprendizaje * (recompensa - puntajeActual);

    // 3. Ajustar el factor de anticipación con el error horizontal observado (si usó estrategia con anticipación)
    if (estrategia == 1 && resultado != 1) {
        float ajuste = (errorX / 500.0f) * tasaAprendizaje;
        factorAnticipacion += ajuste;

        if (factorAnticipacion < 0.0f) factorAnticipacion = 0.0f;
        if (factorAnticipacion > 1.5f) factorAnticipacion = 1.5f;
    }

    // 4. Guardar experiencia en historial limitado (últimos N lanzamientos)
    historial.push_back(Experiencia(contexto, estrategia, resultado, distanciaMinima, errorX, recompensa));
    if (historial.size() > maxHistorial) {
        historial.pop_front();
    }
}

const std::vector<float>& Aprendizaje::obtenerPuntajes(short contexto) const {
    if (contexto < 0 || contexto >= static_cast<short>(puntajes.size())) {
        return puntajes[0];
    }
    return puntajes[contexto];
}

float Aprendizaje::getFactorAnticipacion() const {
    return factorAnticipacion;
}

void Aprendizaje::setTasaAprendizaje(float nuevaTasa) {
    tasaAprendizaje = nuevaTasa;
}

const std::deque<Experiencia>& Aprendizaje::getHistorial() const {
    return historial;
}
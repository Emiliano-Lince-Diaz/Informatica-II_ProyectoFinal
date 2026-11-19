#include "Dificultad.h"

Dificultad::Dificultad(short nivel) {
    configurar(nivel);
}

Dificultad::~Dificultad() {
}

void Dificultad::configurar(short n) {
    nivel = n;

    if (nivel == 1) {
        // Fácil
        multiplicadorCorriente = 1.0f;
        cadenciaLanzamiento = 3.0f;    // Dispara cada 3 segundos
        velocidadObstaculos = 120.0f;
        densidadTrampas = 0.5f;
        radioVortice = 60.0f;
        tasaAprendizaje = 0.15f;       // Aprende lento
    } else if (nivel == 2) {
        // Normal
        multiplicadorCorriente = 1.4f;
        cadenciaLanzamiento = 2.0f;    // Dispara cada 2 segundos
        velocidadObstaculos = 180.0f;
        densidadTrampas = 1.0f;
        radioVortice = 85.0f;
        tasaAprendizaje = 0.30f;       // Aprendizaje estándar
    } else {
        // Difícil (nivel 3 o superior)
        multiplicadorCorriente = 1.8f;
        cadenciaLanzamiento = 1.2f;    // Dispara muy seguido
        velocidadObstaculos = 250.0f;
        densidadTrampas = 1.5f;
        radioVortice = 110.0f;
        tasaAprendizaje = 0.50f;       // Aprende y corrige rápido
    }
}

short Dificultad::obtenerNivel() const {
    return nivel;
}

float Dificultad::getMultiplicadorCorriente() const { return multiplicadorCorriente; }
float Dificultad::getCadenciaLanzamiento() const { return cadenciaLanzamiento; }
float Dificultad::getVelocidadObstaculos() const { return velocidadObstaculos; }
float Dificultad::getDensidadTrampas() const { return densidadTrampas; }
float Dificultad::getRadioVortice() const { return radioVortice; }
float Dificultad::getTasaAprendizaje() const { return tasaAprendizaje; }
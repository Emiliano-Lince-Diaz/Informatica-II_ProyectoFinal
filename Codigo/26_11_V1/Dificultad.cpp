#include "Dificultad.h"

Dificultad::Dificultad(short nivel) {
    configurar(nivel);
}

Dificultad::~Dificultad() {
}

void Dificultad::configurar(short n) {
    nivel = n;

    if (nivel == 1) {
        // Fácil: disparos muy espaciados, pocas trampas, aprendizaje lento
        multiplicadorCorriente = 1.0f;
        cadenciaLanzamiento = 4.2f;
        velocidadObstaculos = 110.0f;
        densidadTrampas = 0.4f;
        radioVortice = 60.0f;
        tasaAprendizaje = 0.08f;
    } else if (nivel == 2) {
        // Normal: equilibrado
        multiplicadorCorriente = 1.4f;
        cadenciaLanzamiento = 2.6f;
        velocidadObstaculos = 175.0f;
        densidadTrampas = 0.8f;
        radioVortice = 85.0f;
        tasaAprendizaje = 0.22f;
    } else {
        // Difícil: disparos frecuentes y aprendizaje rápido
        multiplicadorCorriente = 1.8f;
        cadenciaLanzamiento = 1.5f;
        velocidadObstaculos = 240.0f;
        densidadTrampas = 1.2f;
        radioVortice = 110.0f;
        tasaAprendizaje = 0.45f;
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
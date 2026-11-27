#ifndef APRENDIZAJE_H
#define APRENDIZAJE_H

#include "Experiencia.h"
#include <vector>
#include <deque>
#include <cstddef>

class Aprendizaje {
private:
    std::vector<std::vector<float>> puntajes; // 5 contextos x 4 estrategias
    float factorAnticipacion;
    float tasaAprendizaje;
    std::deque<Experiencia> historial;
    std::size_t maxHistorial;

public:
    Aprendizaje(float tasa = 0.25f, std::size_t maxHist = 20);
    ~Aprendizaje();

    float calcularRecompensa(short resultado, float distanciaMinima) const;
    void aprender(short contexto, short estrategia, short resultado, float distanciaMinima, float errorX);

    const std::vector<float>& obtenerPuntajes(short contexto) const;
    float getFactorAnticipacion() const;
    void setTasaAprendizaje(float nuevaTasa);
    const std::deque<Experiencia>& getHistorial() const;
};

#endif // APRENDIZAJE_H
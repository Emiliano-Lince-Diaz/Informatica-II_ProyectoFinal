#ifndef RAZONAMIENTO_H
#define RAZONAMIENTO_H

#include "Percepcion.h"
#include <vector>

class Razonamiento {
private:
    float epsilon;            // Probabilidad de exploración aleatoria
    float gravedad;
    float imprecision;        // Dispersión en píxeles según la dificultad
    short estrategiaElegida;  // 0: directo, 1: anticipación, 2: ángulo alto, 3: ángulo bajo
    float v0Calculada;
    float anguloCalculado;    // En radianes

public:
    Razonamiento(float epsilon = 0.25f, float gravedad = 980.0f, float imprecision = 65.0f);
    ~Razonamiento();

    short determinarContexto(const Percepcion& p) const;
    short elegirEstrategia(const std::vector<float>& puntajesContexto);
    void calcularTiro(float origenX, float origenY, const Percepcion& p, short estrategia, float factorAnticipacion);

    short getEstrategiaElegida() const;
    float getV0Calculada() const;
    float getAnguloCalculado() const;
    void setEpsilon(float nuevoEpsilon);
    void setImprecision(float nuevaImprecision);
};

#endif // RAZONAMIENTO_H
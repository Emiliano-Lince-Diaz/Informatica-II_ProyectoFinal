#ifndef RESBALADERO_H
#define RESBALADERO_H

#include "Trampa.h"

class Resbaladero : public Trampa {
private:
    float coeficienteFriccion;
    float duracionDeslizamiento;

public:
    Resbaladero(float x = 0.0f, float y = 0.0f, float ancho = 70.0f, float alto = 14.0f, float coef = 0.08f, float duracion = 0.45f);
    ~Resbaladero() override;

    void aplicarEfecto(Personaje& p, float dt) override;
    float getCoeficienteFriccion() const;
    float getDuracionDeslizamiento() const;
};

#endif // RESBALADERO_H
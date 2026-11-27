#ifndef LODAZAL_H
#define LODAZAL_H

#include "Trampa.h"

class Lodazal : public Trampa {
private:
    float coeficienteViscosidad;

public:
    Lodazal(float x = 0.0f, float y = 0.0f, float ancho = 90.0f, float alto = 16.0f, float coef = 0.65f);
    ~Lodazal() override;

    void aplicarEfecto(Personaje& p, float dt) override;
    float getCoeficienteViscosidad() const;
};

#endif // LODAZAL_H
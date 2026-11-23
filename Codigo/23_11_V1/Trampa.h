#ifndef TRAMPA_H
#define TRAMPA_H

#include "Entidad.h"
#include "ModeloFisico.h"

class Personaje;

class Trampa : public Entidad {
protected:
    short dano;
    float radio;
    ModeloFisico* modelo;

public:
    Trampa(float x = 0.0f, float y = 0.0f, float ancho = 40.0f, float alto = 20.0f, short dano = 0, float radio = 0.0f);
    virtual ~Trampa() override;

    void actualizar(float dt) override;
    virtual void aplicarEfecto(Personaje& p, float dt) = 0;

    short getDano() const;
    float getRadio() const;
    ModeloFisico* getModelo() const;
};

#endif // TRAMPA_H
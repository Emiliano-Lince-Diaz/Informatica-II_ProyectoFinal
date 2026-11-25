#ifndef GIFT_H
#define GIFT_H

#include "Entidad.h"

class Personaje; // Declaración adelantada para evitar dependencia circular

class Gift : public Entidad {
protected:
    bool recogido;
    float velocidadCaida;

public:
    Gift(float x = 0.0f, float y = 0.0f, float velCaida = 80.0f);
    virtual ~Gift() override;

    // Método polimórfico de Entidad
    void actualizar(float dt) override;

    // Método virtual puro para aplicar el poder específico al jugador
    virtual void aplicarEfecto(Personaje& p) = 0;

    bool fueRecogido() const;
    void marcarRecogido();
    void setVelocidadCaida(float vel);
};

#endif // GIFT_H
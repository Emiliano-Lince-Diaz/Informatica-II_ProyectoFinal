#ifndef VIBORA_H
#define VIBORA_H

#include "Trampa.h"

class Vibora : public Trampa {
private:
    float tiempoAturdimiento;

public:
    Vibora(float x = 0.0f, float y = 0.0f, float tiempoAturdir = 1.5f);
    ~Vibora() override;

    void aplicarEfecto(Personaje& p, float dt) override;
    float getTiempoAturdimiento() const;
};

#endif // VIBORA_H
#ifndef CAZADOR_H
#define CAZADOR_H

#include "Enemigo.h"

// Enemigo del nivel 2 (tribu en la orilla). Usa el mismo AgenteLanzador que el Mono:
// percibe, razona, lanza un proyectil parabólico y aprende de cada resultado.
class Cazador : public Enemigo {
private:
    short orilla;            // 0: orilla izquierda, 1: orilla derecha
    float distanciaOrilla;   // distancia horizontal entre el cazador y el borde del agua

public:
    Cazador(float x, float y, short orilla, const Dificultad& d, float distanciaOrilla = 40.0f);
    ~Cazador() override;

    void actualizar(float dt, Personaje& p) override;
    Gift* soltarGift() override;

    short getOrilla() const;
    float getDistanciaOrilla() const;
    bool estaEnPantalla() const;
};

#endif // CAZADOR_H

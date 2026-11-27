#ifndef NIVEL_H
#define NIVEL_H

#include "Personaje.h"
#include "Dificultad.h"
#include "Enemigo.h"
#include "Trampa.h"
#include "Gift.h"
#include <vector>

class Nivel {
protected:
    short numero;
    float tiempoMeta;
    float tiempoTranscurrido;
    bool completado;
    bool fallido;
    Personaje* jugador;
    Dificultad* dificultad;
    std::vector<Enemigo*> enemigos;
    std::vector<Trampa*> trampas;
    std::vector<Gift*> gifts;

    float tiempoDesdeChequeoGift;
    float cooldownContactoEnemigo;

public:
    Nivel(Personaje* p, short nivelDificultad, short numNivel = 1, float tMeta = 90.0f);
    virtual ~Nivel();

    virtual void cargar() = 0;
    virtual void actualizar(float dt);
    virtual bool verificarVictoria() = 0;

    void actualizarTemporizador(float dt);
    void resolverColisiones(float dt);
    bool verificarDerrota();
    void limpiarInactivos();

    short getNumero() const;
    float getTiempoMeta() const;
    float getTiempoTranscurrido() const;
    float getTiempoRestante() const;
    bool estaCompletado() const;
    bool estaFallido() const;
    Personaje* getJugador() const;
    const std::vector<Enemigo*>& getEnemigos() const;
    const std::vector<Trampa*>& getTrampas() const;
    const std::vector<Gift*>& getGifts() const;
};

#endif // NIVEL_H
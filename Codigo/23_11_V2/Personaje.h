#ifndef PERSONAJE_H
#define PERSONAJE_H

#include "Entidad.h"

class Gift;

class Personaje : public Entidad {
private:
    short vidas;
    short salud;
    short saltosMaximos;
    short saltosRestantes;
    bool enSuelo;
    bool invulnerable;
    float tiempoInvulnerable;
    bool aturdido;
    float tiempoAturdido;
    bool pesado;
    float factorVelocidad;
    float velocidadBase;
    Gift* giftDisponible;

    // Constantes físicas de salto y gravedad
    float fuerzaSalto;
    float gravedad;

public:
    Personaje(float x = 0.0f, float y = 0.0f);
    ~Personaje() override;

    // Método polimórfico de Entidad
    void actualizar(float dt) override;

    // Movimiento y físicas
    void moverHorizontal(short dir);
    void moverVertical(short dir);
    void saltar();
    void aterrizar(float ySuelo);
    void aplicarGravedad(float dt);

    // Salud, estados y características
    void recibirDano(short cantidad);
    void recogerGift(Gift* g);
    void consumirGift();
    void aturdir(float seg);
    void activarInvulnerabilidad(float seg);
    void activarFortachon(float seg);
    void activarDobleSalto();
    void aplicarPeso();
    void modificarFactorVelocidad(float nuevoFactor);
    void reaparecer(float nx, float ny);

    // Getters de estado para Nivel, Interfaz y Percepción del Agente
    bool estaVivo() const;
    short estadoActual() const;
    bool isEnSuelo() const;
    bool isAturdido() const;
    bool isInvulnerable() const;
    bool isPesado() const;
    bool tieneGiftDisponible() const;
    short getVidas() const;
    short getSalud() const;
    void setEnElSuelo(bool estado);
};

#endif // PERSONAJE_H
#ifndef PERSONAJE_H
#define PERSONAJE_H

#include "Entidad.h"

class Personaje : public Entidad {
private:
    short vidas;
    short salud;
    float velocidadNormal;
    float velocidadActual;

    // Estados de Kapax
    bool esFuerte;
    float tiempoFuerte;
    bool estadoPesado;
    bool dobleSaltoDisponible;
    bool estaAturdido;
    float tiempoAturdido;

    // Físicas propias (Plataformero)
    float fuerzaSalto;
    float gravedad;
    bool enElSuelo;
    short saltosRealizados;

public:
    Personaje(float x = 0.0f, float y = 0.0f);
    ~Personaje() override;

    // Método polimórfico de Entidad
    void actualizar(float dt) override;

    // Movimiento
    void moverHorizontal(float dir);
    void moverVertical(float dir); // Se usará en el Nivel 2 (río)
    void saltar();
    void aplicarGravedad(float dt);

    // Gestión de estados y daño
    void recibirDano(short cant);
    void aturdir(float seg);
    void activarFortachon(float seg);
    void aplicarPeso();
    void activarDobleSalto();
    void reaparecer(float nx, float ny);

    // Setters / Getters auxiliares
    void setEnElSuelo(bool estado);
    bool isAturdido() const;
    short getVidas() const;
    short getSalud() const;
};

#endif // PERSONAJE_H
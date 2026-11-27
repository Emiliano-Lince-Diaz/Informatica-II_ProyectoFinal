#ifndef JUEGO_H
#define JUEGO_H

#include "Personaje.h"
#include "Nivel.h"

class Juego {
private:
    short nivelActual;
    short dificultad;
    bool pausado;
    bool terminado;
    bool victoriaTotal;
    Personaje* kapax;
    Nivel* nivel;

public:
    Juego(short dificultad = 2);
    ~Juego();

    void iniciar(short nivelInicial = 1);
    void cargarNivel(short n);
    void actualizar(float dt);
    void siguienteNivel();
    void reiniciarNivel();
    void pausar();
    void setDificultad(short nuevaDificultad);
    bool juegoTerminado() const;

    short getNivelActual() const;
    short getDificultad() const;
    bool estaPausado() const;
    bool hayVictoria() const;
    Personaje* getKapax() const;
    Nivel* getNivel() const;
};

#endif // JUEGO_H
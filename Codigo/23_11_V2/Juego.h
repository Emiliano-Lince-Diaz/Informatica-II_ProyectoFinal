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

    // Por defecto inicia en el Nivel 1; tu compañero puede llamar iniciar(2) para probar el Nivel 2 directo
    void iniciar(short nivelInicial = 1);
    void cargarNivel(short n);
    void actualizar(float dt);
    void siguienteNivel();
    void reiniciarNivel();
    void pausar();
    bool juegoTerminado() const;

    // Getters para la interfaz gráfica (MainWindow)
    short getNivelActual() const;
    short getDificultad() const;
    bool estaPausado() const;
    bool hayVictoria() const;
    Personaje* getKapax() const;
    Nivel* getNivel() const;
};

#endif // JUEGO_H
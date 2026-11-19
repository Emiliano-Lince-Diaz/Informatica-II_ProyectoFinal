#ifndef DIFICULTAD_H
#define DIFICULTAD_H

class Dificultad {
private:
    short nivel;
    float multiplicadorCorriente;
    float cadenciaLanzamiento;
    float velocidadObstaculos;
    float densidadTrampas;
    float radioVortice;
    float tasaAprendizaje;

public:
    Dificultad(short nivel = 1);
    ~Dificultad();

    void configurar(short nivel);
    short obtenerNivel() const;

    // Getters para que Nivel, Enemigo y AgenteLanzador lean los parámetros
    float getMultiplicadorCorriente() const;
    float getCadenciaLanzamiento() const;
    float getVelocidadObstaculos() const;
    float getDensidadTrampas() const;
    float getRadioVortice() const;
    float getTasaAprendizaje() const;
};

#endif // DIFICULTAD_H
#ifndef ENTIDAD_H
#define ENTIDAD_H

class Entidad {
protected:
    float x;
    float y;
    float vx;
    float vy;
    float ancho;
    float alto;
    bool activo;

public:
    Entidad(float x = 0.0f, float y = 0.0f, float ancho = 0.0f, float alto = 0.0f);
    virtual ~Entidad();

    // Método virtual puro para hacer la clase abstracta y forzar polimorfismo
    virtual void actualizar(float dt) = 0;

    void mover(float dx, float dy);
    bool colisionaCon(const Entidad& otra) const;
    void desactivar();

    // Métodos de acceso (Getters y Setters) necesarios para la capa lógica y gráfica
    float getX() const;
    float getY() const;
    float getVx() const;
    float getVy() const;
    float getAncho() const;
    float getAlto() const;
    bool estaActivo() const;

    void setX(float nuevoX);
    void setY(float nuevoY);
    void setVx(float nuevoVx);
    void setVy(float nuevoVy);
    void setActivo(bool estado);
};

#endif // ENTIDAD_H
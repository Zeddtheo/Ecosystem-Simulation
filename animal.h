#ifndef ANIMAL_H
#define ANIMAL_H

#include "qmatrix4x4.h"
#include "qopenglshaderprogram.h"
#include "qvectornd.h"
class Animal
{
public:
    enum class Type { Herbivore, Carnivore };
    enum class Gender { Male, Female };
    Animal();
    Animal(QVector3D position, Type type, Gender gender);

    Type getType() const;
    Gender getGender() const;

    void reproduce();
    void update();
    void show(QOpenGLShaderProgram *sp, int numTriangles);
    void setPosition(const QVector3D& newPosition);
    const QVector3D& getPosition();
private:
    QVector3D position;
    Type type;
    Gender gender;
    double size;
    std::string color;
    QMatrix4x4 modelMatrix;
};

#endif // ANIMAL_H

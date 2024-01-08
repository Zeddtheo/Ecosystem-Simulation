#include "animal.h"

Animal::Animal() {}


Animal::Animal(QVector3D position, Type type, Gender gender):  position(position), type(type), gender(gender)
{
    if (type == Type::Herbivore) {
        size = 1.0;
    } else {
        size = 1.5;
    }

    if (gender == Gender::Male) {
        color = "Red";
    } else {
        color = "Yellow";
    }
}

void Animal::show(QOpenGLShaderProgram *sp, int numTriangles)
{
    this->modelMatrix.setToIdentity();
    //std::cout<<"x: "<<position.x()<<" y: "<<position.y()<<std::endl;
    this->modelMatrix.translate(position);

    this->modelMatrix.scale(size);

    sp->setUniformValue("modelMatrix", this->modelMatrix);

    QVector3D colorVector;
    if (color == "Red") {
        colorVector = QVector3D(1, 0, 0);
    } else if (color == "Yellow") {
        colorVector = QVector3D(1, 1, 0);
    }

    sp->setUniformValue("color", colorVector);

    glDrawArrays(GL_TRIANGLES, 0, numTriangles * 3);
}


Animal::Type Animal::getType() const
{
    return type;
}

Animal::Gender Animal::getGender() const
{
    return gender;
}

void Animal::setPosition(const QVector3D& newPosition)
{
    position = newPosition;
}


#include "plant.h"
#include <ostream>

Plant::Plant()
{

}

Plant::Plant(QVector3D position): position(position), age(0), life(4), reproductionCycle(3), readyToReproduce(false) {

}

bool Plant::isReadyToReproduce() const
{
    return readyToReproduce;
}

bool Plant::isDead() const
{
    return age >= life;
}

void Plant::resetReproductionCycle()
{
    readyToReproduce = false;
}

void Plant::update()
{
    age++;
    if (age % reproductionCycle == 0)
    {
        readyToReproduce = true;
    }
}

void Plant::show(QOpenGLShaderProgram *sp)
{
    //std::cout<<"x: "<<position.x()<<" y: "<<position.y()<<std::endl;
    this->modelMatrix.setToIdentity();
    this->modelMatrix.translate(position);
    sp->setUniformValue("modelMatrix", this->modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}





#ifndef PLANT_H
#define PLANT_H

#include "qopenglshaderprogram.h"
#include "qvectornd.h"
class Plant {
public:
    Plant();
    Plant(QVector3D position);

    bool isDead() const;
    bool isReadyToReproduce() const;
    void resetReproductionCycle();

    void show(QOpenGLShaderProgram *program);
    void update();
private:
    QVector3D position;
    bool alive;
    int age;
    int life;
    int reproductionCycle;

    bool readyToReproduce;

    QMatrix4x4 modelMatrix;
};

#endif // PLANT_H

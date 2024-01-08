#ifndef GRID_H
#define GRID_H

#include "animal.h"
#include "plant.h"
#include "qopenglshaderprogram.h"
struct GridStatus {
    bool isEmpty;
    bool hasPlant;
    bool hasAnimal;
};
class Grid
{
public:
    Grid();   
    Grid(int sizeGrid, QVector3D position);
    GridStatus getStatus() const;
    const QVector3D& getPosition() const;
    //Plants
    void createPlant();
    Plant* getPlant() const;
    void removePlant();
    void updatePlant();
    bool hasPlant() const;
    //Animals
    void createAnimal(Animal::Type type, Animal::Gender gender);
    void setAnimal(std::shared_ptr<Animal> newAnimal);
    std::shared_ptr<Animal>& getAnimal();
    void removeAnimal();
    bool hasAnimal() const;
    void updateAnimal();

    void show(QOpenGLShaderProgram *sp);
private:
    int sizeGrid;
    QVector3D position;

    std::shared_ptr<Plant> plant;
    std::shared_ptr<Animal> animal;
    QMatrix4x4 modelMatrix;
};

#endif // GRID_H

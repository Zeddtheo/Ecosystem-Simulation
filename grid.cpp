#include "grid.h"
#include <iostream>
#include <ostream>

Grid::Grid()
{

}

Grid::Grid(int sizeGrid, QVector3D position) : sizeGrid(sizeGrid), position(position)
{

}

GridStatus Grid::getStatus() const
{
    GridStatus status;
    status.isEmpty = (plant == nullptr) && (animal == nullptr);
    status.hasPlant = (plant != nullptr);
    status.hasAnimal = (animal != nullptr);
    return status;
}

void Grid::createPlant() {
    plant = std::make_shared<Plant>(position);
}

Plant* Grid::getPlant() const
{
    return plant.get();
}

bool Grid::hasPlant() const {
    return plant != nullptr;
}

void Grid::removePlant()
{
    plant.reset();
}

void Grid::updatePlant()
{
    if (plant != nullptr) {
        plant->update();

        if (plant->isDead()) {
            plant.reset();
        }
    }
}

void Grid::createAnimal(Animal::Type type, Animal::Gender gender)
{
    animal = std::make_shared<Animal>(position, type, gender);
}

std::shared_ptr<Animal>& Grid::getAnimal() {
    return animal;
}

void Grid::setAnimal(std::shared_ptr<Animal> newAnimal) {
    animal = std::move(newAnimal);
}

bool Grid::hasAnimal() const {
    return animal != nullptr;
}

void Grid::removeAnimal()
{
    animal.reset();
}

void Grid::updateAnimal()
{
    if (animal != nullptr) {
        std::string type = (animal->getType() == Animal::Type::Herbivore) ? "Herbivore" : "Carnivore";
        std::string gender = (animal->getGender() == Animal::Gender::Male) ? "Male" : "Female";

        float posX = position.x();
        float posY = position.y();
        float posZ = position.z();

        std::cout << "Animal in Grid at Position (" << posX << ", " << posY << ", " << posZ
                  << "): Type = " << type << ", Gender = " << gender << std::endl;

    }
}

void Grid::show(QOpenGLShaderProgram *sp)
{
    this->modelMatrix.setToIdentity();
    this->modelMatrix.translate(position);

    sp->setUniformValue("modelMatrix", this->modelMatrix);
    sp->setUniformValue("gridSize", this->sizeGrid);

    glDrawArrays(GL_LINES, 0, 8);
}

const QVector3D& Grid::getPosition() const
{
    return position;
}

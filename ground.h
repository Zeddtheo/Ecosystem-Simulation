#ifndef GROUND_H
#define GROUND_H

#include <random>
#include <vector>
#include "Grid.h"

class Ground {
public:
    Ground();
    Ground(int width, int height, int gridSize);
    //Manage grids
    void initializeGrids();
    Grid& getGrid(int x, int y);
    std::vector<std::reference_wrapper<Grid>> getAllGrids();
    std::vector<std::reference_wrapper<Grid>> getAdjacentGrids(int x, int y);
    void show(QOpenGLShaderProgram *sp);
    void updateAllGrids();

    //Manage plants
    void updatePlants();
    void reproducePlants();
    void placeNewPlantNear(int x, int y);
    const std::vector<std::shared_ptr<Plant>>& getPlants() const;
    void removePlantFromGrid(const std::shared_ptr<Plant>& plant);

    //Manage animals
    void moveAnimals();
    void moveAnimalIfPossible(int x, int y);
    bool isValidGrid(int x, int y) const;
    void reproduceAnimals();
    void reproduceInAdjacentGrids(int x, int y, Animal::Type type, std::mt19937& gen);
    void eatPlant(Grid& currentGrid, Grid& targetGrid);
    void eatHerbivore(Grid& currentGrid, Grid& targetGrid);
    void moveAnimal(Grid& fromGrid, Grid& toGrid);
private:
    int width, height, gridSize;
    std::vector<Grid> grids;
    Animal::Type type;
};

#endif

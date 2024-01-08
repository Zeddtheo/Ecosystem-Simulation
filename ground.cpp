#include "ground.h"
#include <random>
#include <unordered_set>

Ground::Ground(){

}

Ground::Ground(int width, int height, int gridSize) : width(width), height(height), gridSize(gridSize)
{
    initializeGrids();
}

void Ground::initializeGrids()
{
    grids.reserve(width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QVector3D position((x - width / 2.0f) * gridSize, (y - height / 2.0f) * gridSize, 0);
            grids.emplace_back(gridSize, position);
        }
    }
    //initial plants
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, width * height - 1);
    std::unordered_set<int> chosenGrids;

    while (chosenGrids.size() < 10) {
        int gridIndex = dis(gen);
        if (chosenGrids.insert(gridIndex).second) {
            grids[gridIndex].createPlant();
        }
    }
    //initial animals
    std::unordered_set<int> chosenAnimalGrids;
    while (chosenAnimalGrids.size() < 4) {
        int gridIndex = dis(gen);
        if (chosenAnimalGrids.insert(gridIndex).second) {
            Animal::Type type = (chosenAnimalGrids.size() <= 2) ? Animal::Type::Carnivore : Animal::Type::Herbivore;
            Animal::Gender gender = (chosenAnimalGrids.size() % 2 == 1) ? Animal::Gender::Male : Animal::Gender::Female;
            grids[gridIndex].createAnimal(type, gender);
        }
    }
}

void Ground::show(QOpenGLShaderProgram *sp)
{
    for (auto& grid : grids){
        grid.show(sp);
    }
}

Grid& Ground::getGrid(int x, int y)
{
    if (x >= 0 && x < width && y >= 0 && y < height){
        return grids[y * width + x];
    } else{
        throw std::out_of_range("Grid coordinates out of range");
    }
}

std::vector<std::reference_wrapper<Grid>> Ground::getAllGrids()
{
    std::vector<std::reference_wrapper<Grid>> allGrids;
    allGrids.reserve(width * height);
    for (auto& grid : grids) {
        allGrids.push_back(std::ref(grid));
    }
    return allGrids;
}

std::vector<std::reference_wrapper<Grid>> Ground::getAdjacentGrids(int x, int y)
{
    std::vector<std::reference_wrapper<Grid>> adjacent;

    std::vector<std::pair<int, int>> directions = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    for (const auto& dir : directions) {
        int adjX = x + dir.first;
        int adjY = y + dir.second;

        if (adjX >= 0 && adjX < width && adjY >= 0 && adjY < height) {
            adjacent.push_back(grids[adjY * width + adjX]);
        }
    }

    return adjacent;
}

void Ground::reproducePlants()
{
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Grid& grid = getGrid(x, y);
            if (grid.hasPlant() && grid.getPlant()->isReadyToReproduce()) {
                auto adjacentGrids = getAdjacentGrids(x, y);
                std::vector<std::reference_wrapper<Grid>> emptyAdjacentGrids;

                for (auto& adjGrid : adjacentGrids) {
                    if (!adjGrid.get().hasPlant()) {
                        emptyAdjacentGrids.push_back(adjGrid);
                    }
                }

                if (!emptyAdjacentGrids.empty()) {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> distrib(0, emptyAdjacentGrids.size() - 1);
                    int randomIndex = distrib(gen);
                    Grid& targetGrid = emptyAdjacentGrids[randomIndex].get();

                    targetGrid.createPlant();

                    grid.getPlant()->resetReproductionCycle();
                }
            }
        }
    }
}

void Ground::moveAnimals()
{
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            moveAnimalIfPossible(x, y);
        }
    }
}

void Ground::moveAnimalIfPossible(int x, int y)
{
    Grid& currentGrid = getGrid(x, y);
    if (!currentGrid.hasAnimal()) {
        return;
    }

    Animal::Type animalType = currentGrid.getAnimal()->getType();

    // static const std::vector<std::pair<int, int>> directions = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
    std::vector<std::pair<int, int>> directions = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(directions.begin(), directions.end(), g);
    for (const auto& dir : directions) {
        int newX = x + dir.first;
        int newY = y + dir.second;

        if (isValidGrid(newX, newY)) {
            Grid& targetGrid = getGrid(newX, newY);

            if (animalType == Animal::Type::Herbivore && targetGrid.hasPlant()) {
                eatPlant(currentGrid, targetGrid);
                return;
            }

            if (animalType == Animal::Type::Carnivore && targetGrid.hasAnimal() && targetGrid.getAnimal()->getType() == Animal::Type::Herbivore) {
                eatHerbivore(currentGrid, targetGrid);
                return;
            }

            if (!targetGrid.hasAnimal()) {
                moveAnimal(currentGrid, targetGrid);
                return;
            }
        }
    }
}

bool Ground::isValidGrid(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

void Ground::reproduceAnimals()
{
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Grid& currentGrid = getGrid(x, y);
            if (currentGrid.hasAnimal()) {
                Animal::Type type = currentGrid.getAnimal()->getType();
                if (type == Animal::Type::Herbivore || type == Animal::Type::Carnivore) {
                    auto adjacentGrids = getAdjacentGrids(x, y);

                    for (auto& adjGrid : adjacentGrids) {
                        Grid& adjacent = adjGrid.get();
                        if (adjacent.hasAnimal() && adjacent.getAnimal()->getType() == type) {
                            if (currentGrid.getAnimal()->getGender() != adjacent.getAnimal()->getGender()) {
                                reproduceInAdjacentGrids(x, y, type, gen);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Ground::reproduceInAdjacentGrids(int x, int y, Animal::Type type, std::mt19937& gen)
{
    auto adjacentGrids = getAdjacentGrids(x, y);
    std::vector<std::reference_wrapper<Grid>> emptyGrids;
    qDebug("here");
    for (auto& adjGrid : adjacentGrids) {
        if (!adjGrid.get().hasAnimal()) {
            emptyGrids.push_back(adjGrid);
        }
    }

    if (!emptyGrids.empty()) {
        std::uniform_int_distribution<> dis(0, emptyGrids.size() - 1);
        int randomIndex = dis(gen);
        Grid& targetGrid = emptyGrids[randomIndex].get();

        Animal::Gender gender = dis(gen) % 2 == 0 ? Animal::Gender::Male : Animal::Gender::Female;
        targetGrid.createAnimal(type, gender);
    }
}


void Ground::eatPlant(Grid& currentGrid, Grid& targetGrid)
{
    targetGrid.removePlant();
    moveAnimal(currentGrid, targetGrid);
}

void Ground::eatHerbivore(Grid& currentGrid, Grid& targetGrid)
{
    targetGrid.removeAnimal();
    moveAnimal(currentGrid, targetGrid);
}

void Ground::moveAnimal(Grid& fromGrid, Grid& toGrid)
{
    QVector3D newPosition = toGrid.getPosition();
    fromGrid.getAnimal()->setPosition(newPosition);
    toGrid.setAnimal(std::move(fromGrid.getAnimal()));
    fromGrid.removeAnimal();
}

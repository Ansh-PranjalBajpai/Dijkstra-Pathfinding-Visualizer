#include <SFML/Graphics.hpp>
#include <queue>
#include <vector>
#include <cmath>
#include <iostream>

const int rows = 60;
const int cols = 60;
const int cellSize = 10;

struct Cell {
    int x, y;
    float cost;
    bool isWall = false;
    bool visited = false;
    Cell* parent = nullptr;

    Cell(int x, int y) : x(x), y(y), cost(INFINITY) {}
};

// Comparison for priority queue
struct CompareCost {
    bool operator()(Cell* a, Cell* b) {
        return a->cost > b->cost;
    }
};

std::vector<Cell*> getNeighbors(Cell* cell, std::vector<std::vector<Cell>>& grid) {
    std::vector<Cell*> neighbors;
    int x = cell->x;
    int y = cell->y;

    if (x > 0) neighbors.push_back(&grid[x - 1][y]);
    if (x < rows - 1) neighbors.push_back(&grid[x + 1][y]);
    if (y > 0) neighbors.push_back(&grid[x][y - 1]);
    if (y < cols - 1) neighbors.push_back(&grid[x][y + 1]);

    return neighbors;
}

void dijkstra(std::vector<std::vector<Cell>>& grid, Cell* start, Cell* end) {
    std::priority_queue<Cell*, std::vector<Cell*>, CompareCost> pq;
    start->cost = 0;
    pq.push(start);

    while (!pq.empty()) {
        Cell* current = pq.top();
        pq.pop();

        if (current->visited) continue;
        current->visited = true;

        if (current == end) break;

        for (Cell* neighbor : getNeighbors(current, grid)) {
            if (neighbor->isWall || neighbor->visited) continue;

            float newCost = current->cost + 1; // All edges have cost 1
            if (newCost < neighbor->cost) {
                neighbor->cost = newCost;
                neighbor->parent = current;
                pq.push(neighbor);
            }
        }
    }
}

void drawPath(Cell* end, sf::RenderWindow& window) {
    Cell* current = end;
    while (current->parent != nullptr) {
        sf::RectangleShape pathSegment(sf::Vector2f(cellSize - 2, cellSize - 2));
        pathSegment.setPosition(current->y * cellSize + 1, current->x * cellSize + 1);
        pathSegment.setFillColor(sf::Color::Yellow);
        window.draw(pathSegment);
        current = current->parent;
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(cols * cellSize, rows * cellSize), "Dijkstra Pathfinding Visualization");

    // Create grid
    std::vector<std::vector<Cell>> grid;
    for (int i = 0; i < rows; ++i) {
        std::vector<Cell> row;
        for (int j = 0; j < cols; ++j) {
            row.emplace_back(i, j);
        }
        grid.push_back(row);
    }

    // Define start and end points
    Cell* start = &grid[0][0];
    Cell* end = &grid[rows - 1][cols - 1];

    bool isRunning = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                int x = mousePos.y / cellSize;
                int y = mousePos.x / cellSize;
                if (x >= 0 && x < rows && y >= 0 && y < cols && !isRunning) {
                    grid[x][y].isWall = true;
                }
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                int x = mousePos.y / cellSize;
                int y = mousePos.x / cellSize;
                if (x >= 0 && x < rows && y >= 0 && y < cols && !isRunning) {
                    grid[x][y].isWall = false;
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !isRunning) {
                isRunning = true;
                dijkstra(grid, start, end);
            }
        }

        window.clear();

        // Draw grid
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                sf::RectangleShape cellShape(sf::Vector2f(cellSize - 2, cellSize - 2));
                cellShape.setPosition(j * cellSize + 1, i * cellSize + 1);

                if (&grid[i][j] == start) {
                    cellShape.setFillColor(sf::Color::Green);
                } else if (&grid[i][j] == end) {
                    cellShape.setFillColor(sf::Color::Red);
                } else if (grid[i][j].isWall) {
                    cellShape.setFillColor(sf::Color::Black);
                } else if (grid[i][j].visited) {
                    cellShape.setFillColor(sf::Color::Blue);
                } else {
                    cellShape.setFillColor(sf::Color::White);
                }

                window.draw(cellShape);
            }
        }

        if (isRunning) {
            drawPath(end, window);
        }

        window.display();
    }

    return 0;
}

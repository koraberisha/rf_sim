// File: rf_visualizer.cpp

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>

const int CELL_SIZE = 6; // Size of each cell in pixels
const int GRID_WIDTH = 100;
const int GRID_HEIGHT = 100;

std::vector<std::vector<double>> loadSignalStrength(const std::string& filename) {
    std::vector<std::vector<double>> data(GRID_HEIGHT, std::vector<double>(GRID_WIDTH, 0.0));
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Error opening signal strength file." << std::endl;
        exit(1);
    }

    std::string line;
    int y = 0;
    while (std::getline(inFile, line) && y < GRID_HEIGHT) {
        std::istringstream iss(line);
        double value;
        int x = 0;
        while (iss >> value && x < GRID_WIDTH) {
            data[y][x] = value;
            x++;
        }
        y++;
    }
    inFile.close();
    return data;
}

sf::Color getColorFromSignal(double signal) {
    if (std::isnan(signal)) {
        return sf::Color(128, 128, 128); // Gray for obstacles
    } else {
        // Map signal strength to color (red for low, green for high)
        double minSignal = -100.0;
        double maxSignal = 0.0;
        double normalized = (signal - minSignal) / (maxSignal - minSignal);
        int r = static_cast<int>((1.0 - normalized) * 255);
        int g = static_cast<int>(normalized * 255);
        return sf::Color(r, g, 0);
    }
}

int main() {
    // Load signal strength data
    std::vector<std::vector<double>> signalData = loadSignalStrength("signal_strength.txt");

    // Create the window
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE), "RF Signal Strength Visualizer");

    // Create a rectangle shape for each cell
    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));

    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Draw the grid
        window.clear();
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                double signal = signalData[y][x];
                cell.setFillColor(getColorFromSignal(signal));
                cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                window.draw(cell);
            }
        }
        window.display();
    }

    return 0;
}


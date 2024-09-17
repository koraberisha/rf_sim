// File: rf_simulator.cpp

#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <fstream>

const int GRID_WIDTH = 1000;
const int GRID_HEIGHT = 1000;
const double TX_POWER = 95.0; // Transmit power in dBm
const double PATH_LOSS_EXPONENT = 4.0; // Free space path loss exponent
const double OBSTACLE_ATTENUATION = 25.0; // Attenuation in dB

struct Point {
    int x, y;
};

struct AccessPoint {
    Point location;
    double txPower;
};

class Environment {
public:
    Environment(int width, int height);
    void addObstacle(int x, int y);
    void addAccessPoint(int x, int y, double txPower);
    void simulate();
    void saveSignalStrength(const std::string& filename);

private:
    int width, height;
    std::vector<std::vector<bool>> obstacles;
    std::vector<std::vector<double>> signalStrength;
    std::vector<AccessPoint> accessPoints;

    bool isObstacle(int x, int y);
    bool isValid(int x, int y);
    double calculateSignalStrength(Point receiver, AccessPoint ap);
    bool hasObstacleBetween(Point a, Point b);
};

Environment::Environment(int w, int h) : width(w), height(h) {
    obstacles.resize(height, std::vector<bool>(width, false));
    signalStrength.resize(height, std::vector<double>(width, 0.0));
}

void Environment::addObstacle(int x, int y) {
    if (isValid(x, y)) {
        obstacles[y][x] = true;
    }
}

void Environment::addAccessPoint(int x, int y, double txPower) {
    if (isValid(x, y)) {
        accessPoints.push_back({{x, y}, txPower});
    }
}

bool Environment::isValid(int x, int y) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Environment::isObstacle(int x, int y) {
    return isValid(x, y) && obstacles[y][x];
}

bool Environment::hasObstacleBetween(Point a, Point b) {
    int dx = abs(b.x - a.x);
    int dy = abs(b.y - a.y);
    int x = a.x;
    int y = a.y;
    int n = 1 + dx + dy;
    int x_inc = (b.x > a.x) ? 1 : -1;
    int y_inc = (b.y > a.y) ? 1 : -1;
    int error = dx - dy;
    dx *= 2;
    dy *= 2;

    for (; n > 0; --n) {
        if (isObstacle(x, y)) return true;

        if (error > 0) {
            x += x_inc;
            error -= dy;
        } else {
            y += y_inc;
            error += dx;
        }
    }
    return false;
}

double Environment::calculateSignalStrength(Point receiver, AccessPoint ap) {
    double distance = std::hypot(ap.location.x - receiver.x,
                                 ap.location.y - receiver.y);
    if (distance == 0) return ap.txPower;

    double pathLoss = 10 * PATH_LOSS_EXPONENT * std::log10(distance);
    double signal = ap.txPower - pathLoss;

    if (hasObstacleBetween(ap.location, receiver)) {
        signal -= OBSTACLE_ATTENUATION;
    }
    return signal;
}

void Environment::simulate() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (isObstacle(x, y)) continue;
            double totalSignal = 0.0;
            Point receiver = {x, y};
            for (auto& ap : accessPoints) {
                totalSignal += std::pow(10, calculateSignalStrength(receiver, ap) / 10.0);
            }
            if (totalSignal > 0) {
                signalStrength[y][x] = 10 * std::log10(totalSignal);
            }
        }
    }
}

void Environment::saveSignalStrength(const std::string& filename) {
    std::ofstream outFile(filename);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (isObstacle(x, y)) {
                outFile << "NaN ";
            } else {
                outFile << signalStrength[y][x] << " ";
            }
        }
        outFile << "\n";
    }
    outFile.close();
}

int main() {
    Environment env(GRID_WIDTH, GRID_HEIGHT);

    // Add obstacles (Example: a building in the middle)
    for (int y = 20; y < 60; ++y) {
        for (int x = 20; x < 60; ++x) {
            env.addObstacle(x, y);
        }
    }

    // Add access points
    env.addAccessPoint(10, 10, TX_POWER+12);
    env.addAccessPoint(90, 90, TX_POWER);

    // Run simulation
    env.simulate();

    // Save signal strength data to a file
    env.saveSignalStrength("signal_strength.txt");

    std::cout << "Simulation complete. Data saved to signal_strength.txt" << std::endl;
    return 0;
}


#ifndef OPTICS_UTILS_H
#define OPTICS_UTILS_H

#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <cmath>
#include <iostream>

// Estructura Point
struct Point {
    double x, y, z, a, b, c, d;
    double reachability_distance = std::numeric_limits<double>::infinity();
    bool processed = false;
    int clusterId = -1;
};


// Distancia euclidiana entre dos puntos
double euclidean_distance(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    double dz = a.z - b.z;
    double da = a.a - b.a;
    double db = a.b - b.b;
    double dc = a.c - b.c;
    double dd = a.d - b.d;
    return sqrt(dx * dx + dy * dy + dz * dz + da * da + db * db + dc * dc + dd * dd);
}

// Función para leer el archivo de datos y añade solo las dos primeras columnas a un vector de puntos
std::vector<Point> readData(std::string filename) {
    std::vector<Point> points;
    std::ifstream file(filename);
    std::string line;
    if(file.is_open()) {
        while (getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            Point p;
            p.x = std::stod(tokens[0]);
            p.y = std::stod(tokens[1]);
            p.z = std::stod(tokens[2]);
            p.a = std::stod(tokens[3]);
            p.b = std::stod(tokens[4]);
            p.c = std::stod(tokens[5]);
            p.d = std::stod(tokens[6]);
            points.push_back(p);
        }
    } else {
        std::cout << "Error al abrir el archivo" << std::endl;
    }
    return points;
}

// Obtiene los vecinos de un punto
std::vector<Point*> get_neighbors(Point& point, std::vector<Point>& points, double eps) {
    std::vector<Point*> neighbors;
    for (Point& p : points) {
        if (euclidean_distance(point, p) <= eps) {
            neighbors.push_back(&p);
        }
    }
    return neighbors;
}

#endif //OPTICS_UTILS_H
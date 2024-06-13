#include <iostream>
#include "KD-Tree.h"
#include "utils.h"
#include <limits>
#include <GL/glut.h>
#include <chrono>

// Estructura Cluster
struct Cluster {
    std::vector<Point> points;
};

// Actualiza la distancia de alcanzabilidad de un punto
void update(Point& point, std::vector<Point*>& neighbors, double eps, int MinPts, std::vector<Point*>& seeds) {
    // Ordenar los vecinos por distancia
    std::sort(neighbors.begin(), neighbors.end(), [&point](Point* a, Point* b) {
        double distance_a = euclidean_distance(point, *a);
        double distance_b = euclidean_distance(point, *b);
        return distance_a < distance_b;
    });
    // Calcular la distancia del núcleo
    double core_distance = (neighbors.size() >= MinPts) ? euclidean_distance(point, *neighbors[MinPts - 1]) : std::numeric_limits<double>::infinity();
    for (Point* n : neighbors) {
        if (!n->processed) {
            double new_reachability_distance = std::max(core_distance, euclidean_distance(point, *n));
            // Actualizar la distancia de alcanzabilidad si el nuevo valor es menor
            if (new_reachability_distance < n->reachability_distance) {
                n->reachability_distance = new_reachability_distance;
                auto it = std::find(seeds.begin(), seeds.end(), n);
                if (it == seeds.end()) {
                    seeds.push_back(n);
                } else {
                    // Actualizar la posición del punto en la lista de semillas si su distancia de alcanzabilidad ha cambiado
                    seeds.erase(it);
                }
                seeds.insert(std::upper_bound(seeds.begin(), seeds.end(), n, [](Point* a, Point* b) {
                    return a->reachability_distance < b->reachability_distance;
                }), n);
            }
        }
    }
}

// Algoritmo OPTICS
std::vector<Point> optics(std::vector<Point>& points, double eps, int MinPts) {
    std::vector<Point> ordered;
    for (Point& p : points) {
        if (!p.processed) {
            std::vector<Point*> neighbors = get_neighbors(p, points, eps);
            p.processed = true;
            ordered.push_back(p);
            if (neighbors.size() >= MinPts) {
                std::vector<Point*> seeds;
                update(p, neighbors, eps, MinPts, seeds);
                while (!seeds.empty()) {
                    Point* q = *std::min_element(seeds.begin(), seeds.end(), [](Point* a, Point* b) {
                        return a->reachability_distance < b->reachability_distance;
                    });
                    std::vector<Point*> q_neighbors = get_neighbors(*q, points, eps);
                    q->processed = true;
                    ordered.push_back(*q);
                    if (q_neighbors.size() >= MinPts) {
                        update(*q, q_neighbors, eps, MinPts, seeds);
                    }
                    seeds.erase(std::find(seeds.begin(), seeds.end(), q));
                }
            }
        }
    }
    return ordered;
}
// Algoritmo OPTICS con KD-Tree
std::vector<Point> opticsK(KDTree& tree, double eps, int MinPts) {
    std::vector<Point> ordered;
    std::vector<Point> points = tree.get_points();
    for (Point& p : points) {
        if (!p.processed) {
            std::vector<Point*> neighbors = tree.get_neighbors(p, eps);
            p.processed = true;
            ordered.push_back(p);
            if (neighbors.size() >= MinPts) {
                std::vector<Point*> seeds;
                update(p, neighbors, eps, MinPts, seeds);
                while (!seeds.empty()) {
                    Point* q = *std::min_element(seeds.begin(), seeds.end(), [](Point* a, Point* b) {
                        return a->reachability_distance < b->reachability_distance;
                    });
                    std::vector<Point*> q_neighbors = tree.get_neighbors(*q, eps);
                    q->processed = true;
                    ordered.push_back(*q);
                    if (q_neighbors.size() >= MinPts) {
                        update(*q, q_neighbors, eps, MinPts, seeds);
                    }
                    seeds.erase(std::find(seeds.begin(), seeds.end(), q));
                }
            }
        }
    }
    return ordered;
}

// Definir los puntos y los clusters
std::vector<Point> points = readData("data.txt");
// Definir los colores de los clusters
std::vector<Cluster> clusters;
// Definir colores para los clusters
float colors[50][3] = {{0.50, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0},
                      {1.0, 1.0, 0.0}, {1.0, 0.0, 1.0},  {0.0, 1.0, 1.0},
                      {0.5, 0.0, 0.0}, {0.0, 0.5, 0.0},  {0.0, 0.0, 0.5},
                      {0.5, 0.5, 0.0}, {0.5, 0.0, 0.5},  {0.0, 0.5, 0.5},
                      {0.5, 0.5, 0.5}, {0.25, 0.0, 0.0}, {0.0, 0.25, 0.0},
                      {0.0, 0.0, 0.25}, {0.25, 0.25, 0.0}, {0.25, 0.0, 0.25},
                      {0.0, 0.25, 0.25}, {0.25, 0.25, 0.25}, {0.75, 0.0, 0.0},
                      {0.0, 0.75, 0.0}, {0.0, 0.0, 0.75}, {0.75, 0.75, 0.0},
                      {0.75, 0.0, 0.75}, {0.0, 0.75, 0.75}, {0.75, 0.75, 0.75},
                      {0.125, 0.0, 0.0}, {0.0, 0.125, 0.0}, {0.0, 0.0, 0.125},
                      {0.125, 0.125, 0.0}, {0.125, 0.0, 0.125}, {0.0, 0.125, 0.125},
                      {0.125, 0.125, 0.125}, {0.375, 0.0, 0.0}, {0.0, 0.375, 0.0},
                      {0.0, 0.0, 0.375}, {0.375, 0.375, 0.0}, {0.375, 0.0, 0.375},
                      {0.0, 0.375, 0.375}, {0.375, 0.375, 0.375}, {0.625, 0.0, 0.0},
                      {0.0, 0.625, 0.0}, {0.0, 0.0, 0.625}, {0.625, 0.625, 0.0},
                      {0.625, 0.0, 0.625}, {0.0, 0.625, 0.625}, {0.625, 0.625, 0.625},
                      {0.875, 0.0, 0.0}, {0.0, 0.875, 0.0}};

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);
    // Ejecutar el algoritmo OPTICS
    double eps = 0.2;
    int MinPts = 40;
    std::sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
    return a.x < b.x;  // Ordenar por la primera dimensión  
    });
    // Creamos el KD-Tree
    KDTree tree(points);
    // Obtenemos los puntos del KD-Tree
    std::vector<Point> KDPoints = tree.get_points();
    std::cout<<"El numero de puntos es: "<<KDPoints.size()<<std::endl;
    // Ejecutamos el algoritmo OPTICS
    auto start = std::chrono::high_resolution_clock::now();

    //------
    // OPTICS con KD-Tree
    //std::vector<Point> ordered = opticsK(tree, eps, MinPts);
    //------
    // OPTICS sin KD-Tree
    std::vector<Point> ordered = optics(points, eps, MinPts);

    auto end = std::chrono::high_resolution_clock::now();
    // Definir el número de clusters
    int num_clusters = 0;
    // Imprimimos el numero de puntos
    std::cout << "Numero de puntos: " << ordered.size() << std::endl;
    // Mostramos los puntos separados por clusters
    for (Point& p : ordered) {
        if (p.clusterId != -1) continue;  // Si el punto ya ha sido asignado a un cluster, continuar con el siguiente punto
        num_clusters++;
        Cluster cluster;
        cluster.points.push_back(p);
        p.clusterId = num_clusters;
        for (Point& q : ordered) {
            if (q.clusterId == -1 && q.reachability_distance <= eps) {  // Si el punto no ha sido asignado a un cluster y su distancia de alcanzabilidad es menor o igual a eps
                cluster.points.push_back(q);
                q.clusterId = num_clusters;
            }
        }
        clusters.push_back(cluster);
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Tiempo de ejecucion: " << duration.count() << " ms" << std::endl;
    // Imprimimos el numero de clusters
    std::cout << "Numero de clusters: " << clusters.size() << std::endl;
    for (const Cluster& cluster : clusters) {
        glColor3fv(colors[(&cluster - &clusters[0]) % 50]);  // Tomar el módulo con respecto al tamaño del array de colores
        glPointSize(2.5);
        for (const Point& point : cluster.points) {
            glBegin(GL_POINTS);
            glVertex2f(point.x, point.y);
            glEnd();
        }
    }
    glFlush();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Optics_Visualizer");
    glutDisplayFunc(display);
    glutMainLoop(); 
    return 0;
}

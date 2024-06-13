#ifndef OPTICS_KDTREE_H
#define OPTICS_KDTREE_H

#include "utils.h"
#include <memory>
#include <unordered_set>
#include <stack>
#include <queue>
#include <algorithm>

// Estructura para el nodo del KD-Tree
struct KDNode {
    Point point;
    KDNode* left;
    KDNode* right;
};

struct SearchInfo {
    KDNode* node;
    int depth;
};

struct NodeInfo {
    std::vector<Point> points;
    KDNode** nodeRef;
    int depth;
};

// Clase para el KD-Tree
class KDTree {
private:
    KDNode* root;
    std::vector<Point> KDPoints;

    // Función que contruye el KD-Tree 
    KDNode* build(std::vector<Point>& points) {
        if (points.empty()) {
            return nullptr;
        }

        KDNode* root = nullptr;
        std::stack<NodeInfo> stack;
        stack.push({points, &root, 0});

        while (!stack.empty()) {
            NodeInfo current = stack.top();
            stack.pop();

            int axis = current.depth % 7;
            std::sort(current.points.begin(), current.points.end(), [axis](const Point& a, const Point& b) {
                return axis == 0 ? a.x < b.x : axis == 1 ? a.y < b.y : axis == 2 ? a.z < b.z : axis == 3 ? a.a < b.a : axis == 4 ? a.b < b.b : axis == 5 ? a.c < b.c : a.d < b.d;
            });

            int median = current.points.size() / 2;
            *current.nodeRef = new KDNode{current.points[median], nullptr, nullptr};

            std::vector<Point> leftPoints(current.points.begin(), current.points.begin() + median);
            std::vector<Point> rightPoints(current.points.begin() + median + 1, current.points.end());

            if (!rightPoints.empty()) {
                stack.push({rightPoints, &(*current.nodeRef)->right, current.depth + 1});
            }
            if (!leftPoints.empty()) {
                stack.push({leftPoints, &(*current.nodeRef)->left, current.depth + 1});
            }
        }

        return root;
    }
public:
    // Función que contruye el KD-Tree a partir de un vector 
    KDTree (std::vector<Point>& points) {
        KDPoints = points;
        root = build(points);
    }

    // Función que busca un punto en el KD-Tree
    KDNode* search(Point& point) {
        std::stack<SearchInfo> stack;
        stack.push({root, 0});

        while (!stack.empty()) {
            SearchInfo current = stack.top();
            stack.pop();

            if (current.node == nullptr) {
                continue;
            }

            int axis = current.depth % 7;
            double pointValue = axis == 0 ? point.x : axis == 1 ? point.y : axis == 2 ? point.z : axis == 3 ? point.a : axis == 4 ? point.b : axis == 5 ? point.c : point.d;
            double nodeValue = axis == 0 ? current.node->point.x : axis == 1 ? current.node->point.y : axis == 2 ? current.node->point.z : axis == 3 ? current.node->point.a : axis == 4 ? current.node->point.b : axis == 5 ? current.node->point.c : current.node->point.d;

            if (point.x == current.node->point.x && point.y == current.node->point.y && point.z == current.node->point.z && point.a == current.node->point.a && point.b == current.node->point.b && point.c == current.node->point.c && point.d == current.node->point.d) {
                return current.node;
            }

            if (pointValue < nodeValue) {
                stack.push({current.node->left, current.depth + 1});
            } else {
                stack.push({current.node->right, current.depth + 1});
            }
        }

        return nullptr;
    }

    // Función que obtiene los vecinos de un punto
    std::vector<Point*> get_neighbors(Point& point, double eps) {
        std::vector<Point*> neighbors;
        std::unordered_set<Point*> visited;
        std::queue<KDNode*> seeds;
        KDNode* node = search(point);
        if (node == nullptr) {
            return neighbors;
        }
        seeds.push(node);
        while (!seeds.empty()) {
            KDNode* current = seeds.front();
            seeds.pop();
            if (visited.find(&current->point) != visited.end()) {
                continue;
            }
            visited.insert(&current->point);
            if (euclidean_distance(current->point, point) <= eps) {
                neighbors.push_back(&current->point);
                if (current->left != nullptr) {
                    seeds.push(current->left);
                }
                if (current->right != nullptr) {
                    seeds.push(current->right);
                }
            }
        }
        return neighbors;
    }

    // Función que obtiene los puntos del KDTree
    std::vector<Point> get_points() {
        return KDPoints;
    }
};

#endif //OPTICS_KDTREE_H
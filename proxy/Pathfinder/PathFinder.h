#pragma once
#include <unordered_set>
#include <list>
#include "Location.h"
#include <queue>
#include <stack>
#include <thread>
#include <set>
#include <map>
#include "../proton/variant.hpp"
#include "../server.h"

class Pathfinder {
public:
    class Point {
    public:
        int x, y, z;
        Point(int xValue = 0, int yValue = 0, int zValue = 0) {
            x = xValue;
            y = yValue;
            z = zValue;
        }

        bool operator<(const Point& other) const {
            if (x < other.x)
                return true;
            if (x > other.x)
                return false;

            if (y < other.y)
                return true;
            if (y > other.y)
                return false;

            if (z < other.z)
                return true;
            if (z > other.z)
                return false;
            return false;
        }
    };

    int width, height;
    bool diagonal = false,  //Whether to consider diagonal moves.
        wraparound = false; //Whether to allow moving off the map and on to the other side (as in Pac-man).

    std::map<Point, int>* nodes; //Map of coordinates to blocked status
    std::map<Point, int>* open;  //Un-traversed squares
    std::set<Point>* closed;     //Traversed squares
    std::vector<Point> path;     //Stores the latest path

    // Returns a list of the given point's surrounding points.
    std::vector<Point> neighborhood(Point point) {
        std::vector<Point> result;
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (!diagonal && (abs(i) == abs(j)))
                    continue; //Skip diagonals

                if (wraparound) {
                    if (i < 0)
                        i += width;
                    if (i >= width)
                        i -= width;
                    if (j < 0)
                        j += height;
                    if (j >= height)
                        j -= height;
                }

                result.push_back(Point(point.x + i, point.y + j, 0));
            }
        }
        return result;
    }

    //Check that the given point is inside the pathfinding area
    bool inBounds(Point point) {
        return (point.x >= 0 && point.x < width&& point.y >= 0 && point.y < height);
    }

    static void teleportomegalul(int x, int y) {
        vector2_t pos;
        pos.m_x = x;
        pos.m_y = y;

        variantlist_t varlist{ "OnSetPos" };
        varlist[1] = pos;
        g_server->m_world.local.pos = pos;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        g_server->send(true, varlist, g_server->m_world.local.netid, -1);
    }

    //Returns the index of the best path of all valid paths.
    int bestPath(int iteration = 1) {
        std::vector<Point> neighbors;
        std::set<Point>::iterator it;
        std::set<Point>* newClosed = new std::set<Point>;

        for (it = closed->begin(); it != closed->end(); it++) {
            neighbors = neighborhood(*it);
            for (int i = 0; i < neighbors.size(); i++) {
                Point neighbor = neighbors[i];

                if (inBounds(neighbor) && (*open)[neighbor] == -1)
                    return iteration;

                if (inBounds(neighbor) && (*open)[neighbor] == 0 && (*nodes)[neighbor] != 1) {
                    (*open)[neighbor] = iteration + 1;
                    newClosed->insert(neighbor);
                }
            }
        }

        if (iteration > (width * height))
            return iteration;

        delete closed;
        closed = newClosed;

        iteration = bestPath(iteration + 1);
        return iteration;
    }

    // Returns an array of path points for the given location and iteration.
    std::vector<Point> pathArray(Point point, int iteration) {
        if (iteration == 0) {
            std::reverse(path.begin(), path.end());
            return path;
        }

        std::vector<Point> neighbors = neighborhood(point);
        for (int i = 0; i < neighbors.size(); i++) {
            Point neighbor = neighbors[i];

            if (inBounds(neighbor) && (*open)[neighbor] == iteration) {
                path.push_back(neighbor);
                return pathArray(neighbor, iteration - 1);
            }
        }
        std::vector<Point> result;
        return result;
    }

    // Returns an array of path points for the given start and end points.
    std::vector<Point> findPath(Point start, Point end) {
        //Create empty path. Return empty path if...
        std::vector<Point> result;
        //Map not loaded
        if (nodes == nullptr)
            return result;

        //End point blocked
        if ((*nodes)[end] == 1)
            return result;

        //End is same as start
        if (start.x == end.x && start.y == end.y) {
            result.push_back(start);
            return result;
        }

        //Store start & end coordinates with lowest & highest passability, respectively
        open->clear();
        (*open)[start] = 1;
        (*open)[end] = -1;

        //Create a set of closed nodes and add start to it
        closed->clear();
        closed->insert(start);

        result = pathArray(end, bestPath());
        if (result.size() > 0)
            result.push_back(end);
        return result;
    }

    Pathfinder(int w, int h) {
        width = w;
        height = h;
        nodes = new std::map<Point, int>;
        open = new std::map<Point, int>;
        closed = new std::set<Point>;
    }

    ~Pathfinder(void) {
        delete nodes;
        delete open;
        delete closed;
    }
};
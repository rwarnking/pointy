#ifndef _GRAPH_H_
#define _GRAPH_H_

#define MAX_SUB 4

#include <unordered_map>
#include <unordered_set>

#include "instance.h"

class Edge
{
public:

    Edge();
    Edge(int a, int b, CORNER ca, CORNER cb);

    inline bool operator==(const Edge &right) const
    {
        return ((one == right.one && two == right.two) ||
               (two == right.one && one == right.two)) &&
               ((corner_one == right.corner_one && corner_two == right.corner_two) ||
               (corner_two == right.corner_one && corner_one == right.corner_two));
    }

    inline bool operator!=(const Edge &right) const
    {
        return !(*this == right);
    }

    int one, two;
    CORNER corner_one, corner_two;
};

template <class T>
    inline void hash_combine(std::size_t& seed, T const& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

struct EdgeHash
{
    std::size_t operator()(const Edge &e) const
    {
        std::size_t seed = 0;
        hash_combine(seed, std::hash<int>{}(e.one));
        hash_combine(seed, std::hash<int>{}(e.two));
        hash_combine(seed, std::hash<short>{}(e.corner_one));
        hash_combine(seed, std::hash<short>{}(e.corner_two));
        return seed;
    }
};

struct EdgeEqual
{
    bool operator()(const Edge& lhs, const Edge& rhs) const
    {
        return lhs == rhs;
    }
};

class Graph
{
public:

    Graph();
    Graph(const char *filename);
    Graph(Instance *inst);

    std::vector<Point>* Nodes();

    bool HasEdge(int from, int to, CORNER from_corner, CORNER to_corner);

    size_t EdgeCount();
    size_t NodeCount();

    void ConstructGraph();

    // Contains all edges as tuples of indices
    std::unordered_set<Edge, EdgeHash, EdgeEqual> edges;
    Instance *instance;
};

#endif /* _GRAPH_H_ */
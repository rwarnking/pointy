#include "../header/graph.h"
#include "../header/logger.h"

using namespace std;

Graph::Graph()
{
    instance = nullptr;
    edges = unordered_set<Edge, EdgeHash, EdgeEqual>();
}

Graph::Graph(const char *filename)
{
    instance = new Instance(filename, false);
    ConstructGraph();
}

Graph::Graph(Instance *inst)
{
    instance = inst;
    ConstructGraph();
}

std::vector<Point>* Graph::Nodes()
{
    return &instance->points;
}

bool Graph::HasEdge(int from, int to, CORNER from_corner, CORNER to_corner)
{
    return edges.find(Edge(from, to, from_corner, to_corner)) != edges.end();
}

size_t Graph::EdgeCount()
{
    return edges.size();
}

size_t Graph::NodeCount()
{
    return instance->point_count;
}

void Graph::ConstructGraph()
{
    if (instance == nullptr)
        logger::Logger::Println(logger::LEVEL::ERR, "ERROR in \"ConstructGraph()\": instance is nullptr");

    size_t nodes = instance->point_count;
    vector<Point> &points = instance->points;
    
    edges = unordered_set<Edge, EdgeHash, EdgeEqual>();

    for (size_t i = 0; i < nodes; i++)
    {
        for (short a = 0; a < (short)NONE; a++)
        {
            points[i].box.SetCorner(points[i].x, points[i].y, (CORNER)a);

            for (size_t j = i+1; j < nodes; j++)
            {
                Edge edge;
                for (short b = 0; b < (short)NONE; b++)
                {
                    points[j].box.SetCorner(points[j].x, points[j].y, (CORNER)b);
                    if (points[i].box.Intersects(points[j].box))
                    {
                        edge = Edge(i, j, (CORNER)a, (CORNER)b);
                        edges.insert(edge);
                    }
                }
            }
        }
    }
}

Edge::Edge()
{
    one = two = 0;
    corner_one = corner_two = NONE;
}

Edge::Edge(int a, int b, CORNER ca, CORNER cb)
{
    one = a;
    two = b;
    corner_one = ca;
    corner_two = cb;
}
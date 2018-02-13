#include "../header/graph.h"
#include "../header/logger.h"

#include <algorithm>

using namespace std;

Graph::Graph()
{
	instance = Instance();
	edges = EdgeGraph();
	cliques = vector<NodeGraph>();
}

Graph::Graph(const char *filename, bool use_cliques)
{
	instance = Instance(filename, false);
	edges = EdgeGraph();
	ConstructGraph();
	if (use_cliques) {
		cliques = vector<NodeGraph>();
		FindMaxCliques();
	}
}

Graph::Graph(Instance inst, bool use_cliques)
{
	instance = inst;
	edges = EdgeGraph();
	ConstructGraph();
	if (use_cliques) {
		cliques = vector<NodeGraph>();
		FindMaxCliques();
	}
}

std::vector<Point>* Graph::Nodes()
{
	return &instance.points;
}

std::vector<NodeGraph>* Graph::GetCliques()
{
	return &cliques;
}

bool Graph::HasEdge(Node &from, Node &to)
{
	return find_if(edges.begin(), edges.end(), [from,to](const Edge &e) {
		return (from == e.from && to == e.to) || (to == e.to && from == e.from);
	}) != edges.end();
}

bool Graph::HasEdge(Node &node)
{
	return find_if(edges.begin(), edges.end(), [node](const Edge &e) {
		return e.from == node || e.to == node;
	}) != edges.end();
}

NodeGraph Graph::Neighbours(Node &node)
{
	NodeGraph graph = NodeGraph();

	for (Edge edge : edges)
	{
		if (edge.from == node)
			graph.push_back(edge.to);
		else if (edge.to == node)
			graph.push_back(edge.from);
	}

	return graph;
}

NodeGraph Graph::Neighbours(Node & node) const
{
	NodeGraph graph = NodeGraph();

	for (Edge edge : edges)
	{
		if (edge.from == node)
			graph.push_back(edge.to);
		else if (edge.to == node)
			graph.push_back(edge.from);
	}

	sort(graph.begin(), graph.end());

	return graph;
}

size_t Graph::EdgeCount()
{
	return edges.size();
}

size_t Graph::NodeCount()
{
	return instance.point_count;
}

void Graph::ConstructGraph()
{
	size_t nodes = instance.point_count;
	vector<Point> &points = instance.points;

	for (size_t i = 0; i < nodes; i++)
	{
		for (short a = 0; a < (short)NONE; a++)
		{
			instance.SetBox(i, (CORNER)a);
			for (size_t j = i + 1; j < nodes; j++)
			{
				for (short b = 0; b < (short)NONE; b++)
				{
					instance.SetBox(j, (CORNER)b);
					if (points[i].box.Intersects(points[j].box))
					{
						edges.insert(Edge(i, j, points[i].box.corner, points[j].box.corner));
					}
				}
			}
		}
		instance.SetBox(i, NONE);
	}
}

void Graph::FindMaxCliques()
{
	NodeGraph vertices = NodeGraph();
	for (Edge edge : edges)
	{
		if (none_of(vertices.begin(), vertices.end(), [edge](Node node){ return node == edge.from; }))
			vertices.push_back(edge.from);
		if (none_of(vertices.begin(), vertices.end(), [edge](Node node){ return node == edge.to; }))
			vertices.push_back(edge.to);
	}

	NodeGraph compsub = NodeGraph();
	auto start = chrono::high_resolution_clock::now();
	BronKerbosh(compsub, vertices, { {} }, start);
}

void Graph::BronKerbosh(NodeGraph &compsub, NodeGraph cand, NodeGraph cnot, std::chrono::high_resolution_clock::time_point start)
{
	if (cand.empty() && cnot.empty())
	{
		cliques.push_back(compsub);
		return;
	}

	Node fixp = FindFixP(cand);
	Node current = fixp;
	chrono::minutes limit(5);

	while (current.node >= 0) {
		auto time = chrono::high_resolution_clock::now() - start;
		if (chrono::duration_cast<chrono::minutes>(time) >= limit || LimitReached(cliques.size()))
			return;

		NodeGraph new_not;
		NodeGraph new_cand;
		NodeGraph new_compsub;

		// Auxiliar lambda function
		auto is_connected = [current, this](Node x) {
			const NodeGraph& edges = this->Neighbours(x);
			return find(edges.begin(), edges.end(), current) != edges.end();
		};

		new_compsub.reserve(compsub.size() + 1);
		new_not.reserve(cnot.size());
		new_cand.reserve(cand.size());
		copy_if(cnot.begin(), cnot.end(), back_inserter(new_not), is_connected);
		copy_if(cand.begin(), cand.end(), back_inserter(new_cand), is_connected);
		copy(compsub.begin(), compsub.end(), back_inserter(new_compsub));
		new_compsub.push_back(current);

		BronKerbosh(new_compsub, new_cand, new_not, start);

		// Generate new cnot and cand for the loop
		cnot.push_back(current);
		cand.erase(find(cand.begin(), cand.end(), current));

		// Last check
		auto v = find_if(cand.begin(), cand.end(), [fixp, this](Node x) {
			const NodeGraph& edges = this->Neighbours(x);
			return find(edges.begin(), edges.end(), fixp) == edges.end();
		});

		// Obtain new cur_v value
		if (v != cand.end())
			current = *v;
		else
			break;
    }
}

Node Graph::FindFixP(NodeGraph cand) const
{
	vector<int> connections;
	connections.resize(cand.size());

	sort(cand.begin(), cand.end());

	// Auxiliar lambda function
	auto intersection = [&cand, this](Node x) -> int {
		const NodeGraph& edges = this->Neighbours(x);
		NodeGraph intersection;

		set_intersection(edges.begin(), edges.end(),
						cand.begin(), cand.end(),
						back_inserter(intersection));
		return intersection.size();
	};

	// Create an auxiliar vector with the intersection sizes
	transform(cand.begin(), cand.end(), connections.begin(), intersection);

	// Find the maximum size and return the corresponding edge
	vector<Node>::const_iterator it1, itmax;
	vector<int>::const_iterator it2;
	int max = -1;
	itmax = cand.end();
	for (it1 = cand.begin(), it2 = connections.begin(); it1 != cand.end(); ++it1, ++it2)
	{
		if (max < *it2)
		{
			max = *it2;
			itmax = it1;
		}
	}
	if (itmax == cand.end())
		return Node();
	else
		return *itmax;
}

bool Graph::LimitReached(size_t cliques_num)
{
	return cliques_num >= std::min((size_t)500, instance.point_count/3);
}

Edge::Edge() : Edge(-1, -1, NONE, NONE) {}

Edge::Edge(int a, int b, CORNER ca, CORNER cb) : from(Node(a, ca)), to(Node(b, cb)) {}

Edge::Edge(const Edge &other)
{
	this->from = other.from;
	this->to = other.to;
}
Node::Node() : Node(-1, NONE) {}

Node::Node(int node, CORNER corner) : node(node), corner(corner) {}

Node::Node(const Node &other)
{
	this->node = other.node;
	this->corner = other.corner;
}
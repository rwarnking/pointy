#ifndef _GRAPH_H_
#define _GRAPH_H_

#define MAX_SUB 4

#include <unordered_set>
#include <chrono>

#include "instance.h"

class Node
{
public:

	int node;
	CORNER corner;

	Node();
	Node(const Node &other);
	Node(int node, CORNER corner);

	inline bool operator==(const Node &other) const
	{
		return node == other.node && corner == other.corner;
	}

	inline bool operator!=(const Node &other) const
	{
		return !(*this == other);
	}

	inline bool operator<(const Node &other) const
	{
		return node == other.node ? corner < other.corner : node < other.node;
	}

	inline bool operator>(const Node &other) const
	{
		return !(*this == other || *this < other);
	}
};

class Edge
{
public:
	
	Node from, to;
	
	Edge();
	Edge(const Edge &other);
	Edge(int a, int b, CORNER ca, CORNER cb);

	inline bool operator==(const Edge &other) const
	{
		return from == other.from && to == other.to || from == other.to && to == other.from;
	}

	inline bool operator!=(const Edge &right) const
	{
		return !(*this == right);
	}
};

template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct EdgeHash
{
	std::size_t operator()(const Edge &e) const
	{
		std::size_t seed = 0;
		if (e.from.node <= e.to.node)
		{
			hash_combine(seed, std::hash<int>{}(e.from.node));
			hash_combine(seed, std::hash<int>{}(e.to.node));
		}
		else
		{
			hash_combine(seed, std::hash<int>{}(e.to.node));
			hash_combine(seed, std::hash<int>{}(e.from.node));
		}

		if (e.from.corner <= e.to.corner)
		{
			hash_combine(seed, std::hash<short>{}(e.from.corner));
			hash_combine(seed, std::hash<short>{}(e.to.corner));
		}
		else
		{
			hash_combine(seed, std::hash<short>{}(e.to.corner));
			hash_combine(seed, std::hash<short>{}(e.from.corner));
		}
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

typedef std::vector<Node> NodeGraph;
typedef std::unordered_set<Edge, EdgeHash, EdgeEqual> EdgeGraph;

class Graph
{
public:

	Graph();
	Graph(const char *filename, bool use_cliques=false);
	Graph(Instance inst, bool use_cliques=false);

	std::vector<Point>* Nodes();
	std::vector<NodeGraph>* GetCliques();

	bool HasEdge(Node &from, Node &to);
	bool HasEdge(Node &node);

	size_t EdgeCount();
	size_t NodeCount();

	// Contains all edges as tuples of indices
	EdgeGraph edges;
	std::vector<NodeGraph> cliques;
	Instance instance;

private:

	NodeGraph Neighbours(Node &node);
	NodeGraph Neighbours(Node &node) const;
	
	void ConstructGraph();
	void FindMaxCliques();
	void BronKerbosh(NodeGraph &compsub, NodeGraph cand, NodeGraph cnot, std::chrono::high_resolution_clock::time_point start);
	Node FindFixP(NodeGraph cand) const;

	bool LimitReached(size_t clique_num);
};

#endif /* _GRAPH_H_ */
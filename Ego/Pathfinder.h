#pragma once
#include <algorithm>  // Include algorithms
#include <list>
#include <vector>
#include "Node.h"

/// implements A* pathfinding algorithm.
/** This is the pathfinder used by the Actor class to find a path from one point to another in a room.
	If the destination location is unreachable, the pathfinder will find the nearest reachable point, and
	direct the actor to said point.  This uses the collision map as nodes, which is a 80x60 256-color
	(1 byte per pixel) bitmap.
*/
class Pathfinder {
	private:
		bool pathFound; ///< true if a path has been found.
		std::vector<Node> openList; ///< the list of nodes which are, as yet, unexplored
		std::list<Node> closedList; ///< the list of already explored nodes
		std::list<Node*> path; ///< A list of the path found.
		short whichList[80][60]; ///< a 2-d array to quickly determine if a node is on the open or on the closed list.
		char *pathMap; ///< a pointer to the collision map used.

	public:
		// Main Pathfinder function.
		bool FindPath(char *map, long startingX, long startingY, long destX, long destY);

		// Finds the closest reachable node to the starting node.
		POINT FindWalkable(long startingX, long startingY);

		// Function to calculate G-Value of a node.  
		int CalculateGVal(const Node & parent, const Node & test) const;

		// Function calculates the H-Value of a Node.
		int CalculateHVal(long posX, long posY, long destX, long destY) const;

		// Function to test whether a node is the destination.
		bool isDestination(const Node & current, long destX, long destY) const;

		// Uses pathMap to deduce whether a node is walkable.  
		bool isWalkable(const Node & current) const;

		// Function to query whichList.
		bool onClosed(const Node & current) const;
		bool onOpen(const Node & current) const;

		// Function to set the x and y coordinates of Node neighboring current.
		Node GetNeighbor(int index, const Node & current) const;

		// Returns the coordinates of the next node in a path, or -1, -1 (unreachable) if the path is empty.
		POINT GetNextNode();

		// Returns true if the path is not empty.
		bool HasNextNode() const;
};


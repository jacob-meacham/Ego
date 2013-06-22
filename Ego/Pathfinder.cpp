#include "Pathfinder.h"
#include "Framework\Define.h"
using namespace std;

/// Enumeratres the type of node.
enum { unexplored = -1, closed = 0, open = 1 };
//////////////////////////////////////////////////////////////////////////////////
/// Main Pathfinder function.
/** This function finds a path between the two points (or the closest reachable destination),
	and stores the path in the list path.
	\param map The collision map (an 80x60 bitmap)
	\return true if a path is found, false otherwise.
*/
bool Pathfinder::FindPath(char* map, long startingX, long startingY, long destX, long destY) {
	if(startingX == destX && startingY == destY) 
	{ 
		return true; 
	}

	openList.clear();
	closedList.clear();
	path.clear();
	pathMap = map; // set class map
	pathFound = false;
	
	// reset whichList to unexplored
	for (int x = 0; x < 80; x++) {
		for (int y = 0; y < 60; y++) { 
			whichList [x][y] = unexplored; 
		}
	}

	// create starting node
	Node start(startingX/10, 60-startingY/10, 0, 0);
	Node dest(destX/10, 60-destY/10, 0, 0);
	if(!isWalkable(dest)) {
		// if the destination is unwalkable, find the nearest walkable location.
		POINT newLoc = FindWalkable(destX/10, 60-destY/10);
		if(FindPath(pathMap, startingX, startingY, newLoc.x*10, (60-newLoc.y)*10)) {
			return true;
		}
		
		return false;
	}

	//add starting node to open List, and set it as open on whichList
	openList.push_back(start);
	whichList[start.GetX()][start.GetY()] = open;
		
	while(1) { // run until we find a solution
		
		if(pathFound == true) { break; } // break to end if a path is found
	
		// if the open list is ever empty and the path hasn't been found, then there is no path.  return false
		if(openList.empty()) {
			TRACE("No Path\n");
			return false;
		}
	
		// Find the best node
		std::vector<Node>::iterator ibest;
		int best_val = INT_MAX;
		for(std::vector<Node>::iterator i = openList.begin(); i != openList.end(); i++) {
			Node & node = *i;
			if (node.GetFVal() < best_val) {
				ibest = i;
				best_val = node.GetFVal();
			}
		}
		Node & best = *ibest; // grab the lowest F cost node off of the open list
		
		// push the current node onto the closed list and set it to closed on whichList
		closedList.push_back(best); 
		Node & current = closedList.back();

		whichList[best.GetX()][best.GetY()] = closed;
		openList.erase(ibest);		

		// step through each direction sequentially.  This goes clockwise and starts at 1 = top.
		// There is a slight bias towards up and right, but it should not affect the algorithm overly much.
		for(int index = 1; index <= 8; index++) {
			Node test = GetNeighbor(index, current);
			test.SetParent(&current); // set our parent as current (ie the last node pushed onto the closed list).
			test.SetGVal();

			// if at any time the test node is our destination, then push it onto the open list: we have found a path.
			// It is important to note that this is not necessarily the most efficient path -- to find that, we must
			// wait until the destination node is pushed onto the closed list -- but this path is adequate for most
			// purposes, and more quickly found.
			if(isDestination(test, destX/10, 60-(destY/10))) {
				openList.push_back(test);
				pathFound = true;
				break;
			} else if(!isWalkable(test)) { 
				// if the node is unwalkable, we can ignore it
			} else if(onClosed(test)) { 
				// if the node is on the closed list, we ignore it
			} else if(!onOpen(test)) {
				// if it is not yet on the open list, we may calculate H and G and add it to the list
				test.SetHVal(CalculateHVal(test.GetX(), test.GetY(), destX, destY));
				whichList[test.GetX()][test.GetY()] = open;
				openList.push_back(test);
			} else if(onOpen(test)) {
				 // if it is already on the open list, we see if the current path is better
				for(std::vector<Node>::iterator i = openList.begin(); i != openList.end(); i++) {
					Node & node = *i;
					// the current path is better if the current node's G value is lower.
					if(node == test && node.GetGVal() > test.GetGVal()) {
						// if this is the case, we recalculate the F cost, 
						// and change the node's parent to the current node.
						node.SetParent(&current);
						node.SetGVal();
						node.SetHVal(CalculateHVal(test.GetX(), test.GetY(), destX, destY));
						break;
					}
				}
			}
		}			
	}
	
	// if we exit the while loop, then a path has been found: store it in the path list.
	Node *pathStart = &openList.back();

	// the first Node's parent is null, so while there is a parent, add the node, and set
	// pathStart equal to her parent.
	while(pathStart->GetParent() != 0) {
		path.push_back(pathStart);
		pathStart = pathStart->GetParent();
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Function calculates the H-Value of a Node.  
/** the H-value is the heuristic which guesses the distance left between 
	the current node and the destination.  The heuristic in use is the 
	extremely simplistic Manhattan distance with a factor which 
	slightly biases the h-value of nodes added later.  This tie-breaker 
	factor ensures that the algorithm will expand nodes closer to the destination first.
*/
int Pathfinder::CalculateHVal(long posX, long posY, long destX, long destY) const {
	return 10*(abs(posX - destX) + abs(posY - destY)) + .5*(abs(posX-destX) + abs(posY - destY));
}
//////////////////////////////////////////////////////////////////////////////////
/// Function to calculate G-Value of a node.  
/** G-Value represents the cost of moving from the start to this node. 
	A diagonal move is 1.4 times longer than a cardinal move, so a diagonal move costs more.
*/
int Pathfinder::CalculateGVal(const Node & parent, const Node & test) const {
	int addedCost = 0;
		if(abs(test.GetX() - parent.GetX()) == 1 && abs(test.GetY() - parent.GetY()) == 1) {
			addedCost = 14;
		} else { addedCost = 10; }

		return (parent.GetGVal() + addedCost);
}
//////////////////////////////////////////////////////////////////////////////////
/// Function to test whether a node is the destination, using x and y coordinates.
bool Pathfinder::isDestination(const Node & current, long destX, long destY) const {
	if(current.GetX() == destX && current.GetY() == destY) {
		return true;
	} 
	
	return false;
}
//////////////////////////////////////////////////////////////////////////////////
/// Uses pathMap to deduce whether a node is walkable.  
/** If the node is off the map, obviously it is not walkable.
    If the value of the pixel which corresponds to the node is 0, then the node is walkable.
*/
bool Pathfinder::isWalkable(const Node & current) const {
	if(current.GetY() < 0 || current.GetY() > 59 || current.GetX() < 0 || current.GetX() > 79) { return false; }
	if(pathMap[current.GetY() * 80 + current.GetX()] == 0) { return true; }
	return false;
}
//////////////////////////////////////////////////////////////////////////////////
/// queries whichList to determine if current is on the closed list.
bool Pathfinder::onClosed(const Node & current) const {
	if(whichList[current.GetX()][current.GetY()] == closed) { return true; }

	return false;
}
//////////////////////////////////////////////////////////////////////////////////
/// queries whichList to determine if current is on the open list.
bool Pathfinder::onOpen(const Node & current) const {
	if(whichList[current.GetX()][current.GetY()] == open) { return true; }

	return false;
}
//////////////////////////////////////////////////////////////////////////////////
/// Function to set the x and y coordinates of Node neighboring current.
/* \param index 1 = top, 2 = top-right, 3 = right, 4 = bottom-right, 5 = bottom, 6 = bottom-left, 7 = left, 8 = top-left.
*/
Node Pathfinder::GetNeighbor(int index, const Node & current) const {
	Node neighbor;
	switch(index) {
		case 1:
			neighbor.SetX(current.GetX());
			neighbor.SetY(current.GetY() + 1);
			break;
		case 2:
			neighbor.SetX(current.GetX() + 1);
			neighbor.SetY(current.GetY() + 1);
			break;
		case 3:
			neighbor.SetX(current.GetX() + 1);
			neighbor.SetY(current.GetY());
			break;
		case 4:
			neighbor.SetX(current.GetX() + 1);
			neighbor.SetY(current.GetY() - 1);
			break;
		case 5:
			neighbor.SetX(current.GetX());
			neighbor.SetY(current.GetY() - 1);
			break;
		case 6:
			neighbor.SetX(current.GetX() - 1);
			neighbor.SetY(current.GetY() - 1);
			break;
		case 7:
			neighbor.SetX(current.GetX() - 1);
			neighbor.SetY(current.GetY());
			break;
		case 8:
			neighbor.SetX(current.GetX() - 1);
			neighbor.SetY(current.GetY() + 1);
			break;
	}
	return neighbor;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the coordinates of the next node in a path, or -1, -1 (unreachable) if the path is empty.
POINT Pathfinder::GetNextNode() {
	POINT newLoc;
	if(!path.empty()) {
	newLoc.x = (path.back())->GetX();
	newLoc.y = (path.back())->GetY();
	path.pop_back();
	return newLoc;
	}
	else {
		newLoc.x = -1;
		newLoc.y = -1;
		return newLoc;
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns true if the path is not empty.
bool Pathfinder::HasNextNode() const {
	return(!path.empty());
}
//////////////////////////////////////////////////////////////////////////////////
/// Finds the closest reachable node to the starting node.
/** This function searches in a circle, until it finds a node which is walkable.
*/
POINT Pathfinder::FindWalkable(long startingX, long startingY) {
	POINT newLoc;
	Node start(startingX, startingY, 0, 0);
	// the neighbor above the start.
	Node up = GetNeighbor(1, start);
	// the neighbor to the right of the start.
	Node right = GetNeighbor(3, start);
	// the neighbor to the left of the start. 
	Node left = GetNeighbor(7, start);
	// the neighbor below the start.
	Node bottom = GetNeighbor(5, start);
	// continue until a walkable node is found.
	while(true) {
		// find the neighbors of the starts neighbors
		Node tempUp = GetNeighbor(1, up);
		Node tempRight = GetNeighbor(3, right);
		Node tempLeft = GetNeighbor(7, left);
		Node tempBottom = GetNeighbor(5, bottom);

		// if any are walkable, return their coordinates.
		if(isWalkable(tempUp)) {
			newLoc.x = tempUp.GetX();
			newLoc.y = tempUp.GetY();
			return newLoc;
		}

		if(isWalkable(tempRight)) {
			newLoc.x = tempRight.GetX();
			newLoc.y = tempRight.GetY();
			return newLoc;
		}

		if(isWalkable(tempLeft)) {
			newLoc.x = tempLeft.GetX();
			newLoc.y = tempLeft.GetY();
			return newLoc;
		}

		if(isWalkable(tempBottom)) {
			newLoc.x = tempBottom.GetX();
			newLoc.y = tempBottom.GetY();
			return newLoc;
		}

		// otherwise, reset and continue the search.
		up = tempUp;
		right = tempRight;
		left = tempLeft;
		bottom = tempBottom;
	}
}

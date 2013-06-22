#include "Node.h"

/// Default Constructor.
Node::Node() { parent = 0; }

/// Constructor.
/** Parent is set to null. Must call set parent to use this node.
	\param x X-Coordinate of the node.
	\param y Y-Coordinate of the node.
	\param gval G-value of the node.
	\param hval H-value of the node.
*/
Node::Node(int x, int y, int gval, int hval) {
	loc.x = x;
	loc.y = y;
	gValue = gval;
	hValue = hval;
	parent = 0;
}

/// Sets the h-value of the node.
void Node::SetHVal(int val) {
	hValue = val;
}

/// Returns the h-value of the node.
int Node::GetHVal() const {
	return hValue;
}

/// Sets the g-value of the node.  
/** Since the g-value is always the same, based on distance from parent, we can calculate it directly.
*/
void Node::SetGVal() {
	int addedCost = 0;
	if(parent != 0) {
		if(abs(loc.x - parent->GetX()) == 1 && abs(loc.y - parent->GetY()) == 1) {
			addedCost = 14;
		}
		else {
			addedCost = 10;
		}
		gValue = parent->GetGVal() + addedCost;
	}
}

/// Returns the g-value of the node.
int Node::GetGVal() const {
	return gValue;
}

/// Returns the X-Coordinate of the node.
int Node::GetX() const {
	return loc.x;
}

/// Sets the X-Coordinate of the node.
void Node::SetX(int x) {
	loc.x = x;
}

/// Returns the Y-Coordinate of the node.
int Node::GetY() const {
	return loc.y;
}

/// Sets the Y-Coordinate of the node.
void Node::SetY(int y) {
	loc.y = y;
}

/// Sets the parent of this node.
void Node::SetParent(Node *p) {
	parent = p;
}

/// Returns a pointer to the parent of this node.
Node* Node::GetParent() {
	return parent;
}

#pragma once
#include <windows.h>

/// Class that encapsulates a node of the pathfinder.
class Node {
	private:
		POINT	loc;		   ///< Location on the collision map.
		Node	*parent;   ///< Location of the parent node.
		int		gValue;        ///< The g-value in A* represents how far we've already gone.
		int		hValue;        ///< The h-value in A* represents an estimate of how far is left.

	public:
		/// To compare two nodes, we compare the `f' value, which is the
		/// sum of the g and h values.  Sort in descending order.
		bool operator < ( const Node& other) const {
			return (this->gValue + this->hValue) > (other.gValue + other.hValue);
		}

		bool operator > ( const Node& other) const {
			return (this->gValue + this->hValue) < (other.gValue + other.hValue);
		}

		/// Two nodes are equal if their components are equal.
		bool operator == ( const Node& other) const {
			return (this->loc.x == other.loc.x) && (this->loc.y == other.loc.y); 
		}

		// Constructors.
		Node();
		Node(int x, int y, int gval = 0, int hval = 0);
		
		// Setters.
		void SetX(int x);
		void SetY(int y);
		void SetHVal(int val);
		void SetGVal();
		void SetGVal(int val);
		void SetParent(Node *p);
		
		// Getters
		int GetX() const;
		int GetY() const;
		int GetHVal() const;
		int GetGVal() const;
		int GetFVal() const { return GetGVal() + GetHVal(); }
		Node* GetParent();
};
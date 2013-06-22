#pragma once
#include "Object.h"
#include "Pathfinder.h"

/// Default animations
enum DefaultAnimations {
	DEFAULT_TALKING = 0,
	TALKING_RIGHT_DOWN,
	TALKING_RIGHT_UP,
	TALKING_LEFT_DOWN,
	TALKING_LEFT_UP,
	TALKING_BACK,
	TALKING_FRONT,
	STANDING_RIGHT_DOWN,
	STANDING_RIGHT_UP,
	STANDING_LEFT_DOWN,
	STANDING_LEFT_UP,	
	STANDING_BACK,
	STANDING_FRONT,
	WALKING_RIGHT,
	WALKING_LEFT,
	WALKING_BACK,
	WALKING_FRONT,
	PICKING_UP_RIGHT_DOWN,
	PICKING_UP_RIGHT_UP,
	PICKING_UP_LEFT_DOWN,
	PICKING_UP_LEFT_UP,
	PICKING_UP_BACK,
	PICKING_UP_FRONT,
};

/// An actor is any object which may move about the screen.  
/** An actor is any object which may move about the screen.  
  This includes the main character, and any secondary, movable characters/objects.  Note: an NPC that is stationary
  (ie does not need pathfinding) should be declared as an object, as object animation is easier.
*/
class Actor : public Object {

	protected:
		int			m_moveSpeed; ///< Movement speed of the actor.
		Pathfinder  m_pathFinder; 		///< private pathfinder used to navigate around a room.
		ActionType	m_curActionType; ///< Action which the actor is undertaking.
		long		m_curActionX; /// x-coordinate of the actor's current action.
		long		m_curActionY; /// y-coordinate of the actor's current action.
		Object*		m_curActionObject; ///< object on which the current action focuses (if any).	
		POINT		m_Foot; 		///< point describing where the actor's feet are. Used to determine collisions.
		POINT		m_Direction; 	///< eigenvector describing the x,y direction that the actor is facing.
		bool		m_ActionBool; /// A boolean set when the current action coordinates are reached.

	public:	
		// Default constructor.
		Actor();

		// Creates an actor object.
		void CreateActor(char TileNum, const std::string & name, long XPos, long YPos);

		// Sets all current action parameters (use NULL for no current object).
		void SetCurAction(ActionType action, Object * actionObject, long actionX, long actionY);

		// Returns \enum ActionType current action.
		ActionType GetCurAction() const;

		// Performs current action, stored in m_curActionType.
		bool DoCurAction();

		// Returns the x-coordinate of the current action.
		float GetCurActionX() const;

		// Returns the x-coordinate of the current action.
		float GetCurActionY() const;

		// Returns a pointer to the current object (or NULL if there is no current object).
		Object* GetCurActionObject();

		// Setter function to force the current position to become the action coordinates.
		void SetAtCurActionCoord(bool d);

		// returns true if the current coordinates are those of the curAction coordinates.
		bool AtCurActionCoord() const;
		
		// Sets the actor to the correct walking animation as she/he walks a path.
		void UpdateActorWalkingAnimation(long xPos, long yPos);

		// Sets the actor facing the correct direction when she/he has stopped.
		virtual void SetFacingDirection();

		// Updates the actor.
		virtual void Update();

		// Uses the pathfinder class to find a path from the current position to the destination.
		void FindPath(long xPos, long yPos, char *map);

		// Simple function to set the direction vector of the actor such that the correct animation cycle is used.
		void FindDirection(long xPos, long yPos);

		// Function used to move the actor to an coordinate.
		void Move(long xMove, long yMove);

		// Moves the actor to the next pathfinder node.
		void MoveToNextNode();

		// Setter to set an Actor's speed.
		void SetSpeed(int speed);

		// Getter to get an Actor's speed.
		int GetSpeed() const;

		// Setter to set an Actor's foot x, y coordinate.
		void SetFootXYPos(long x, long y);
		
		// Getter to get an Actor's foot x-coordinate.
		long GetFootXPos() const;

		// Getter to get an Actor's foot y-coordinate.
		long GetFootYPos() const;
		
		
};
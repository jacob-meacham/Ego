#include "Actor.h"
//#include "Global.h"
#define Error(x) MessageBox(NULL, x, "Error", MB_OK);

/// Default constructor. Unused.
Actor::Actor() { }
	

/// Creates an actor object.
/** \param TileNum The tilenumber to use from the used tileset (see Sprite for more details).
	\param name The name of the Actor.
	\param XPos The x-coordinate at which the actor will be created.
	\param YPos the y-coordinate at which the actor will be created.
*/
void Actor::CreateActor(char TileNum, std::string name, long XPos, long YPos) {
	Create(TileNum, name, XPos, YPos);
	SetSpeed(7);
	// For most humans, the feet can be placed at the bottom-middle of the sprite.
	SetFootXYPos(GetXPos() + GetWidth()/2, GetYPos() + GetHeight());
}


/// Sets all current action parameters (use NULL for no current object).
void Actor::SetCurAction(ActionType action, Object* actionObject, long actionX, long actionY) {
	m_curActionType = action;
	m_curActionObject = actionObject;
	m_curActionX = actionX;
	m_curActionY = actionY;
	SetAtCurActionCoord(false);
}

/// Returns \enum ActionType current action.
ActionType Actor::GetCurAction() {
	return m_curActionType;
}

/// Performs current action, stored in m_curActionType.
/** This function is called every frame.  It returns true only when
	the current action requires an object to execute a script.
	Otherwise, it either moves the Actor via MoveToNextNode(),
	or updates the Actor's current action.
*/
bool Actor::DoCurAction() {
	switch(GetCurAction()) {
		// If there is no action, then we should not need to animate.
		case IS_NOACTION:
			SetAutoAnimate(false);
			return false;
			break;
		case IS_WALK:
			// If the actor has reached the action coordinates, then
			// set her current action to IS_NOACTION.
			if(AtCurActionCoord()) {
				SetFacingDirection();
				SetCurAction(IS_NOACTION, NULL, 0, 0); 
				return false;
			}
			// Otherwise, move her to the next node via the pathfinder.
			else { 
				MoveToNextNode(); 
				return false;
			}
			break;
		default:
			if(!AtCurActionCoord()) {
				MoveToNextNode();
				return false;
			}
			// If another action is the current action and the actor is at 
			// the action coordinates, then we must process an object script.
			// return true.
			else {
				SetFacingDirection();
				return true;
			}
	}
}

/// Returns the x-coordinate of the current action.
float Actor::GetCurActionX() {
	return m_curActionX;
}

/// Returns the y-coordinate of the current action.
float Actor::GetCurActionY() {
	return m_curActionY;
}

/// Returns a pointer to the current object (or NULL if there is no current object).
Object* Actor::GetCurActionObject() {
	return m_curActionObject;
}

/// Setter function to force the current position to become the action coordinates.
void Actor::SetAtCurActionCoord(bool d) {
	m_ActionBool = d;
}

/// returns true if the current coordinates are those of the curAction coordinates.
bool Actor::AtCurActionCoord() {
	return m_ActionBool;
}

/// Sets the actor to the correct walking animation as she/he walks a path.
void Actor::UpdateActorWalkingAnimation(long xPos, long yPos) {
	SetAutoAnimate(true);
	FindDirection(xPos, yPos);
	if(m_Direction.x == 1 && m_Direction.y == 0 && GetCurAnimation() != WALKING_RIGHT) {
		SetAnimation(WALKING_RIGHT);
	}
	else if(m_Direction.x == 1 && m_Direction.y == -1 && GetCurAnimation() != WALKING_RIGHT) {
		SetAnimation(WALKING_RIGHT);
	}
	else if(m_Direction.x == 0 && m_Direction.y == -1 && GetCurAnimation() != WALKING_FRONT) {
		SetAnimation(WALKING_FRONT);
	}
	else if(m_Direction.x == -1 && m_Direction.y == -1 && GetCurAnimation() != WALKING_LEFT) {
		SetAnimation(WALKING_LEFT);
	}
	else if(m_Direction.x == -1 && m_Direction.y == 0 && GetCurAnimation() != WALKING_LEFT) {
		SetAnimation(WALKING_LEFT);
	}
	else if(m_Direction.x == -1 && m_Direction.y == 1 && GetCurAnimation() != WALKING_LEFT) {
		SetAnimation(WALKING_LEFT);
	}
	else if(m_Direction.x == 0 && m_Direction.y == 1 && GetCurAnimation() != WALKING_BACK) {
		SetAnimation(WALKING_BACK);
	}
	else if(m_Direction.x == 1 && m_Direction.y == 1 && GetCurAnimation() != WALKING_RIGHT) {
		SetAnimation(WALKING_RIGHT);
	}
}

/// Sets the actor facing the correct direction when she/he has stopped.
void Actor::SetFacingDirection() {
		if(m_Direction.x == 1 && m_Direction.y == 0) { 
		SetDefaultAnimation(STANDING_RIGHT_DOWN); 
		SetAnimation(STANDING_RIGHT_DOWN);
		SetCurTalkingAnimation(TALKING_RIGHT_DOWN);
	}
	else if(m_Direction.x == 1 && m_Direction.y == -1) { 
		SetDefaultAnimation(STANDING_RIGHT_DOWN); 
		SetAnimation(STANDING_RIGHT_DOWN);
		SetCurTalkingAnimation(TALKING_RIGHT_DOWN);
	}
	else if(m_Direction.x == 0 && m_Direction.y == -1) { 
		SetDefaultAnimation(STANDING_FRONT); 
		SetAnimation(STANDING_FRONT); 
		SetCurTalkingAnimation(TALKING_FRONT);
	}
	else if(m_Direction.x == -1 && m_Direction.y == -1) { 
		SetDefaultAnimation(STANDING_LEFT_DOWN); 
		SetAnimation(STANDING_LEFT_DOWN);
		SetCurTalkingAnimation(TALKING_LEFT_DOWN);
	}
	else if(m_Direction.x == -1 && m_Direction.y == 0) { 
		SetDefaultAnimation(STANDING_LEFT_DOWN); 
		SetAnimation(STANDING_LEFT_DOWN);
		SetCurTalkingAnimation(TALKING_LEFT_DOWN);
	}
	else if(m_Direction.x == -1 && m_Direction.y == 1) { 
		SetDefaultAnimation(STANDING_LEFT_UP); 
		SetAnimation(STANDING_LEFT_UP);
		SetCurTalkingAnimation(TALKING_LEFT_UP);

	}
	else if(m_Direction.x == 0 && m_Direction.y == 1) { 
		SetDefaultAnimation(STANDING_BACK); 
		SetAnimation(STANDING_BACK); 
		SetCurTalkingAnimation(TALKING_BACK);
	}
	else if(m_Direction.x == 1 && m_Direction.y == 1) { 
		SetDefaultAnimation(STANDING_RIGHT_UP); 
		SetAnimation(STANDING_RIGHT_UP); 
		SetCurTalkingAnimation(TALKING_RIGHT_UP);
	}
}

/// Updates the actor.
void Actor::Update() {
	Sprite::Update();
	//DoCurAction();
}

/// Uses the pathfinder class to find a path from the current position to the destination.
/** \param xPos the destination x-coordinate.
	\param yPos the destination y-coordinate.
	\param map a pointer to the collision bitmap data.
*/
void Actor::FindPath(long xPos, long yPos, char *map) {
	// uses FootXPos and FootYPos because the foot point is from where
	// bipedal actors will move.
	m_pathFinder.FindPath(map, GetFootXPos(), GetFootYPos(), xPos, yPos);
}

/// Simple function to set the direction vector of the actor such that the correct animation cycle is used.
void Actor::FindDirection(long xPos, long yPos) {
	if(abs(GetFootXPos() - xPos) == 0) m_Direction.x = 0;
	else if(xPos > GetFootXPos()) m_Direction.x = 1;
	else m_Direction.x = -1;
	if(abs(GetFootYPos()-yPos) == 0) m_Direction.y = 0;
	else if(yPos < GetFootYPos()) m_Direction.y = 1;
	else m_Direction.y = -1;	
}

/// Function used to move the actor to any coordinate.
/** The preferred method of movement is to use 
SetFootXYPos(), for purposes of pathfinding.
*/
void Actor::Move(long xMove, long yMove) {
	m_Location.x += xMove;
	m_Location.y += yMove;
}

/// Moves the actor to the next pathfinder node.  
/** FindPath() must be called before MoveToNextNode().
*/
void Actor::MoveToNextNode() {
	if(m_pathFinder.HasNextNode()) {
		POINT temp = m_pathFinder.GetNextNode();
		UpdateActorWalkingAnimation(temp.x*10, (60-temp.y)*10);
		SetFootXYPos(temp.x*10, (60-temp.y)*10);
	}
	// if the pathfinder does not have another node, then we must be at the end
	// (ie at the action coordinates).
	else {
		SetAtCurActionCoord(true);
	}

}

/// Setter to set an Actor's speed.
void Actor::SetSpeed(int speed) {
	m_moveSpeed = speed;
}

/// Getter to get an Actor's speed.
int Actor::GetSpeed() {
	return m_moveSpeed;
}

/// Setter to set an Actor's foot x, y coordinate.
void Actor::SetFootXYPos(long x, long y) {
	m_Foot.x = x;
	m_Foot.y = y;
	SetXYPos(x-GetWidth()/2, y-GetHeight());
}

/// Getter to get an Actor's foot x-coordinate.
long Actor::GetFootXPos() {
	return m_Foot.x;
}

/// Getter to get an Actor's foot y-coordinate.
long Actor::GetFootYPos() {
	return m_Foot.y;
}
	
#include "Ego.h"
#include "Inventory.h"
#include "Room.h"
//////////////////////////////////////////////////////////////////////////////////
Ego::Ego() { 
	inventory = new Inventory(); 
	inventory->SetParent(this); 
	curPickupAnimation = PICKING_UP_RIGHT_DOWN;
}
//////////////////////////////////////////////////////////////////////////////////
Ego::~Ego() {
	delete inventory;
}
//////////////////////////////////////////////////////////////////////////////////
void Ego::SetParentRoom(Room* room) {
	curRoom = room;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets himself as the parent of his inventory.
void Ego::SetInventoryParent() { inventory->SetParent(this); }
//////////////////////////////////////////////////////////////////////////////////
/// Getter for the main inventory.
/// \return a pointer to the main inventory.
Inventory* Ego::GetInventory() {
	return inventory;
}
//////////////////////////////////////////////////////////////////////////////////
/// Changes the currently held item
void Ego::SetCurrentlyHeldItem(Object *o) { curHeldItem = o; }
//////////////////////////////////////////////////////////////////////////////////
/// Returns the currently held item.
Object* Ego::GetCurrentlyHeldItem() { return curHeldItem; }
const Object * Ego::GetCurrentlyHeldItem() const { return curHeldItem; }
//////////////////////////////////////////////////////////////////////////////////
/// Sets the actor facing the correct direction when she/he has stopped.
void Ego::SetFacingDirection() {
	if(direction.x == 1 && direction.y == 0) { 
		SetDefaultAnimation(STANDING_RIGHT_DOWN); 
		SetAnimation(STANDING_RIGHT_DOWN);
		SetCurTalkingAnimation(TALKING_RIGHT_DOWN);
		curPickupAnimation = PICKING_UP_RIGHT_DOWN;
	} else if(direction.x == 1 && direction.y == -1) { 
		SetDefaultAnimation(STANDING_RIGHT_DOWN); 
		SetAnimation(STANDING_RIGHT_DOWN);
		SetCurTalkingAnimation(TALKING_RIGHT_DOWN);
		curPickupAnimation = PICKING_UP_RIGHT_DOWN;
	} else if(direction.x == 0 && direction.y == -1) { 
		SetDefaultAnimation(STANDING_FRONT); 
		SetAnimation(STANDING_FRONT); 
		SetCurTalkingAnimation(TALKING_FRONT);
		curPickupAnimation = PICKING_UP_FRONT;
	} else if(direction.x == -1 && direction.y == -1) { 
		SetDefaultAnimation(STANDING_LEFT_DOWN); 
		SetAnimation(STANDING_LEFT_DOWN);
		SetCurTalkingAnimation(TALKING_LEFT_DOWN);
		curPickupAnimation = PICKING_UP_LEFT_DOWN;
	} else if(direction.x == -1 && direction.y == 0) { 
		SetDefaultAnimation(STANDING_LEFT_DOWN); 
		SetAnimation(STANDING_LEFT_DOWN);
		SetCurTalkingAnimation(TALKING_LEFT_DOWN);
		curPickupAnimation = PICKING_UP_LEFT_DOWN;
	} else if(direction.x == -1 && direction.y == 1) { 
		SetDefaultAnimation(STANDING_LEFT_UP); 
		SetAnimation(STANDING_LEFT_UP);
		SetCurTalkingAnimation(TALKING_LEFT_UP);
		curPickupAnimation = PICKING_UP_LEFT_UP;
	} else if(direction.x == 0 && direction.y == 1) { 
		SetDefaultAnimation(STANDING_BACK); 
		SetAnimation(STANDING_BACK); 
		SetCurTalkingAnimation(TALKING_BACK);
		curPickupAnimation = PICKING_UP_BACK;
	} else if(direction.x == 1 && direction.y == 1) { 
		SetDefaultAnimation(STANDING_RIGHT_UP); 
		SetAnimation(STANDING_RIGHT_UP); 
		SetCurTalkingAnimation(TALKING_RIGHT_UP);
		curPickupAnimation = PICKING_UP_RIGHT_UP;
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Performs Ego's current pickup animation, as set in curPickupAnimation.
void Ego::DoPickupAnimation() {
	SetAnimation(curPickupAnimation);
	SetAutoAnimate(true);
}
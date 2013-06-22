#include "Ego.h"
#include "Inventory.h"
#include "Room.h"
#define Error(x) MessageBox(NULL, x, "Error", MB_OK);
/// Default constructor.  Constructs the inventory, and sets its parent as the Ego instance.
Ego::Ego() { 
	m_inventory = new Inventory(); 
	m_inventory->SetParent(this); 
	m_curPickupAnimation = PICKING_UP_RIGHT_DOWN;
}

Ego::~Ego() {
	delete m_inventory;
}

void Ego::SetParentRoom(Room* room) {
	m_curRoom = room;
}

/// Sets himself as the parent of his inventory.
void Ego::SetInventoryParent() { m_inventory->SetParent(this); }

/// Getter for the main inventory.
/// \return a pointer to the main inventory.
Inventory* Ego::GetInventory() {
	return m_inventory;
}

/// Changes the currently held item
void Ego::SetCurrentlyHeldItem(Object *o) { m_curHeldItem = o; }

/// Returns the currently held item.
Object* Ego::GetCurrentlyHeldItem() { return m_curHeldItem; }

/// Sets the actor facing the correct direction when she/he has stopped.
void Ego::SetFacingDirection() {
	if(m_Direction.x == 1 && m_Direction.y == 0) { 
		SetDefaultAnimation(STANDING_RIGHT_DOWN); 
		SetAnimation(STANDING_RIGHT_DOWN);
		SetCurTalkingAnimation(TALKING_RIGHT_DOWN);
		m_curPickupAnimation = PICKING_UP_RIGHT_DOWN;
	}
	else if(m_Direction.x == 1 && m_Direction.y == -1) { 
		SetDefaultAnimation(STANDING_RIGHT_DOWN); 
		SetAnimation(STANDING_RIGHT_DOWN);
		SetCurTalkingAnimation(TALKING_RIGHT_DOWN);
		m_curPickupAnimation = PICKING_UP_RIGHT_DOWN;
	}
	else if(m_Direction.x == 0 && m_Direction.y == -1) { 
		SetDefaultAnimation(STANDING_FRONT); 
		SetAnimation(STANDING_FRONT); 
		SetCurTalkingAnimation(TALKING_FRONT);
		m_curPickupAnimation = PICKING_UP_FRONT;
	}
	else if(m_Direction.x == -1 && m_Direction.y == -1) { 
		SetDefaultAnimation(STANDING_LEFT_DOWN); 
		SetAnimation(STANDING_LEFT_DOWN);
		SetCurTalkingAnimation(TALKING_LEFT_DOWN);
		m_curPickupAnimation = PICKING_UP_LEFT_DOWN;
	}
	else if(m_Direction.x == -1 && m_Direction.y == 0) { 
		SetDefaultAnimation(STANDING_LEFT_DOWN); 
		SetAnimation(STANDING_LEFT_DOWN);
		SetCurTalkingAnimation(TALKING_LEFT_DOWN);
		m_curPickupAnimation = PICKING_UP_LEFT_DOWN;
	}
	else if(m_Direction.x == -1 && m_Direction.y == 1) { 
		SetDefaultAnimation(STANDING_LEFT_UP); 
		SetAnimation(STANDING_LEFT_UP);
		SetCurTalkingAnimation(TALKING_LEFT_UP);
		m_curPickupAnimation = PICKING_UP_LEFT_UP;

	}
	else if(m_Direction.x == 0 && m_Direction.y == 1) { 
		SetDefaultAnimation(STANDING_BACK); 
		SetAnimation(STANDING_BACK); 
		SetCurTalkingAnimation(TALKING_BACK);
		m_curPickupAnimation = PICKING_UP_BACK;
	}
	else if(m_Direction.x == 1 && m_Direction.y == 1) { 
		SetDefaultAnimation(STANDING_RIGHT_UP); 
		SetAnimation(STANDING_RIGHT_UP); 
		SetCurTalkingAnimation(TALKING_RIGHT_UP);
		m_curPickupAnimation = PICKING_UP_RIGHT_UP;
	}
}

/// Performs Ego's current pickup animation, as set in m_curPickupAnimation.
void Ego::DoPickupAnimation() {
	SetAnimation(m_curPickupAnimation);
	SetAutoAnimate(true);
}

Room* Ego::GetRoom() { return m_curRoom; }
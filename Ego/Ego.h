#ifndef _ego_h_included_
#define _ego_h_included_
#include "Actor.h"


class Inventory;
class Room;
/// Wrapper class for the main character, Ego.
/** Contains the main (and only) inventory.
*/
class Ego : public Actor {

	private:
		Inventory*		m_inventory; ///< Pointer to the main inventor.
		Room*			m_curRoom;
		Object*			m_curHeldItem; ///< Pointer to the currently held item.
		int				m_curPickupAnimation; ///< number of the current direction for picking up items.
	public:
		// Constructor.
		Ego();

		void SetParentRoom(Room* room);
		
		// Sets ego facing the correct direction when she/he has stopped.
		virtual void SetFacingDirection();
		
		// Perform pickup animation.
		void DoPickupAnimation();
		
		// Setters.
		void SetInventoryParent();
		void SetCurrentlyHeldItem(Object *o);

		// Getters.
		Inventory* GetInventory();
		Object* GetCurrentlyHeldItem();
		Room* GetRoom();
};
#endif
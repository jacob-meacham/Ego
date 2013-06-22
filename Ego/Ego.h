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
		Inventory*		inventory; ///< Pointer to the main inventor.
		Room*			curRoom;
		Object*			curHeldItem; ///< Pointer to the currently held item.
		int				curPickupAnimation; ///< number of the current direction for picking up items.
	public:
		// Constructor.
		Ego();
		~Ego();

		void SetParentRoom(Room* room);
		
		// Sets ego facing the correct direction when she/he has stopped.
		virtual void SetFacingDirection();
		
		// Perform pickup animation.
		void DoPickupAnimation();

		void SetCurrentlyHeldItem(Object *o);

		// Getters.
		Inventory* GetInventory();
		Object* GetCurrentlyHeldItem();
		const Object* GetCurrentlyHeldItem() const;
		Room * GetRoom() { return curRoom; }
};
#endif
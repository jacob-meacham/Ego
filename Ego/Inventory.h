#ifndef _inventory_h_included_
#define _inventory_h_included_
#include "GameArea.h"
#include "Framework\Texture.h"

class Ego;
class Room;
class GraphicsDevice;
/// Encapsulates the main character's inventory.
/** This class derives from GameArea, and, as such, is able to run scripts.  The only inventory instance
	should be on the main ego instance of the game (see EgoApp)
*/
class Inventory : public GameArea {
	
private:	
		long					m_nextObjectX; ///< X-Coordinate of the next object to be placed in inventory.
		long					m_nextObjectY; ///< Y-Coordinate of the next object to be placed in inventory.
		Ego						*m_parent; ///< Parent Ego (used for FindObject() ).
		Tile					m_inventoryTiles; // Tileset for items in inventory.
		

	public:
		// Constructor.
		Inventory();

		// Sets the parent of this inventory.
		void SetParent(Ego *p);

		// Sets tileset of this inventory.
		void SetInventoryTiles(GraphicsDevice *Graphics, long NumTextures);

		// Adds an object to the main inventory list.
		void AddInventoryObject(Object o);

		// Relays input to the inventory for processing.
		void QueryInventory(long mouseX, long mouseY, bool lClick);

		// Updates a current script by executing the next line and processing input.
		void UpdateScript(long mouseX, long mouseY, bool lClick);
		
		// Renders the inventory.
		void RenderInventory();
		
		// Finds an object in the inventory.
		Object* FindObject(std::string objectName);

		// Removes an object from inventory.
		void LoseObject(std::string objectName);
		
		// Getters.
		Inventory* GetInventory();
		Ego* GetEgo();
		void GetNextObjectCoordinates();
};
#endif
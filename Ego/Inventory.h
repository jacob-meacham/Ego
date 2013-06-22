#pragma once
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
	long nextObjectX; ///< X-Coordinate of the next object to be placed in inventory.
	long nextObjectY; ///< Y-Coordinate of the next object to be placed in inventory.
	Ego	* pParent; ///< Parent Ego (used for FindObject() ).
	Tile inventoryTiles; // Tileset for items in inventory.
	bool bActive;
		
public:
	Inventory(Ego * pparent);

	// Sets tileset of this inventory.
	void SetInventoryTiles(long NumTextures);

	// Adds an object to the main inventory list.
	void AddInventoryObject(const Object & o);

	// Relays input to the inventory for processing.
	void QueryInventory(long mouseX, long mouseY, bool lClick);

	// Updates a current script by executing the next line and processing input.
	void UpdateScript(long mouseX, long mouseY, bool lClick);
		
	// Renders the inventory.
	void RenderInventory() const;
		
	// Finds an object in the inventory.
	Object* FindObject(const std::string & objectName);

	// Removes an object from inventory.
	void LoseObject(const std::string & objectName);

	void GetNextObjectCoordinates();

	void SetActive(bool active) { bActive = active; }
	bool IsActive() { return bActive; }
};
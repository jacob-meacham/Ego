#include "Inventory.h"
#include <string>
#include <sstream>
#include "Room.h"
#include "ScriptParser.h"
#include "Framework\System.h"
//////////////////////////////////////////////////////////////////////////////////
Inventory::Inventory() { 
	SetInScript(false); 
	pParser->SetParent(this);
	nextObjectX = 120; 
	nextObjectY = 30; 
}
//////////////////////////////////////////////////////////////////////////////////
void Inventory::SetParent(Ego *p) { 
	pParent = p; 
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the tileset of this inventory, so that inventory objects can easily move from room to room.
void Inventory::SetInventoryTiles(long NumTextures) {
	inventoryTiles.Create(NumTextures);
}
//////////////////////////////////////////////////////////////////////////////////
/// Adds an object to the main inventory list.
void Inventory::AddInventoryObject(const Object & o) {
	Object & new_object = AddObject(o);

	if(o.GetName().find("Inventory") == string::npos) {
		stringstream s;
		s << o.GetName() << "Inventory";
		new_object.SetName(s.str());
	}

	new_object.SetHasOnStep(false);
	new_object.SetXYPos(nextObjectX, nextObjectY);

	float scale;
	if(o.GetHeight() > o.GetWidth()) {
		scale = (100.0f / (float)o.GetHeight());
	}
	else {
		scale = (100.0f / (float)o.GetWidth());
	}
	
	new_object.SetXScale(scale);
	new_object.SetYScale(scale);
	new_object.SetVisible(true);
	GetNextObjectCoordinates(); // calculate the next coordinates
}
//////////////////////////////////////////////////////////////////////////////////
/// Relays input to the inventory for processing.
/** \param mouseX X-Coordinate of the mouse.
	\param mouseY Y-Coordinate of the mouse.
	\param lClick true if the left mouse button was clicked, false otherwise.
*/
void Inventory::QueryInventory(long mouseX, long mouseY, bool lClick) {
	// if in script, then we do not need to worry about input.
	if(GetInScript()) { 
		UpdateScript(mouseX, mouseY, lClick); 
		return; 
	}

	for(std::list<Object>::iterator iObject = objectList.begin(); iObject != objectList.end(); iObject++) {
		// Check if the mouse is hovering over an object.
		if((*iObject).CheckMouseCollision(mouseX, mouseY)) {
			// if so, we need to render the descriptor.
			if(pParent->GetCurrentlyHeldItem() == 0) {
				SetCursor(LoadCursorFromFile("red-cursor.cur"));
			}

			curMouseObject = (*iObject).GetDescriptor();
			if(lClick) {
				SetCurActionObject(&(*iObject));
				if(pParent->GetCurrentlyHeldItem() != 0) {	
					(*iObject).UseItem(pParent->GetCurrentlyHeldItem()->GetName(), pParser); 
					SetInScript(true);
				}
				else if(GetGlobalAction() == IS_USE) { 
					stringstream cursorFile;
					cursorFile << (*iObject).GetName() << ".cur";
					SetCursor(LoadCursorFromFile((char*)cursorFile.str().c_str()));
					pParent->SetCurrentlyHeldItem(&(*iObject));
				}
				else {
					// If the user also clicked on this object, then we need to process a script.
					(*iObject).DoAction(GetGlobalAction(), pParser);
					// We are now in script.
					SetInScript(true);
				}
				return;
			}
			return;
		}
		curMouseObject.clear(); // otherwise, there is no current object.
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Updates a current script by executing the next line and processing input.
void Inventory::UpdateScript(long mouseX, long mouseY, bool lClick) {
	// if the parser has finished the line and is not waiting, execute the next line
	if(pParser->FinishedLine() && !pParser->WaitingForInput()) {
		pParser->ExecuteNextLine();
	} else if(pParser->WaitingForInput()) {
		// if the parser is waiting, then we are in a conversation.  
		for(std::list<TextBox>::iterator iText = activeChoices.begin(); iText != activeChoices.end(); iText++) {
			// If the mouse is hovering, then we need to change the text color.
			if((*iText).CheckMouseCollision(mouseX, mouseY)) {
				// If the user chose a conversation choice, then we need to jump to that choice.
				(*iText).SetColor(0xFF532FD4);
				if(lClick) {
					pParser->JumpToConversation((*iText).GetChoiceNumber());
					return;
				}
				return;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Renders the inventory, including any objects in the inventory, and any script strings.
void Inventory::RenderInventory() const {
	// First, render the objects.
	for(std::list<Object>::const_iterator iObject = objectList.begin(); iObject != objectList.end(); iObject++) {
		(*iObject).Render();
	}

	if(!GetInScript()) {
		// If we are not in a script, render the descriptor of the current mouse object, if any 
		pFont->render(curMouseObject.c_str(), 0, 570, gSystem.getWidth(), gSystem.getHeight(), 0xFFFFFFFF, DT_CENTER);
	} else if(GetInScript() && !pParser->WaitingForInput()) {
		// If we are in script, render the current conversation string, if any
		pFont->render(curConversationString.c_str(), 0, 250,
			650, gSystem.getHeight(), curConversationStringColor, DT_WORDBREAK | DT_CENTER);
	} else if(GetInScript() && pParser->WaitingForInput()) {
		// if the parser is waiting for input, then render any conversation choices.
		PrintConversationChoices();
	} 
	
	if(pParent->GetCurrentlyHeldItem() != NULL) {
		pFont->render(pParent->GetCurrentlyHeldItem()->GetName().c_str(), 0, 0, gSystem.getWidth(), gSystem.getHeight(), 0xFFFFFFFF);
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Finds an object in the inventory.
/** Because find object is used by Parser, and a script will almost positively have dialogue from Ego,
	the inventory must be able to return its ego parent.  If it is not asked for EGO, then it looks
	through the objectList.  If it is not an inventory item, then it looks through the room's items.
*/
Object* Inventory::FindObject(const std::string & objectName) {
	if(objectName.compare("EGO") == 0) { return pParent; }
	for(std::list<Object>::iterator iObject = objectList.begin(); iObject != objectList.end(); iObject++) {
		if((*iObject).GetName().compare(objectName) == 0) return &(*iObject);
	}
	
	return pParent->GetRoom()->FindObject(objectName);
}
//////////////////////////////////////////////////////////////////////////////////
/// Removes an item from inventory, and retools coordinates.
void Inventory::LoseObject(const std::string & objectName) {
	bool found = false;
	std::list<Object>::iterator tempI;
	for(std::list<Object>::iterator iObject = objectList.begin(); iObject != objectList.end(); iObject++) {
		if((*iObject).GetName().compare(objectName) == 0) {
			if(pParent->GetCurrentlyHeldItem() != 0 && 
			  (*iObject).GetName().compare(pParent->GetCurrentlyHeldItem()->GetName()) == 0) {
				pParent->SetCurrentlyHeldItem(0);
			}
			
			nextObjectX = (*iObject).GetXPos();
			nextObjectY = (*iObject).GetYPos();
			tempI = iObject;
		
			// iterate before the object to be erased.
			if(tempI != objectList.begin()) {
				tempI--;
				objectList.erase(iObject);
				found = true;
				break;
			} else {
				objectList.erase(iObject);
				return;
			}
		}
	}

	if(found) {
		for(tempI; tempI != objectList.begin(); tempI--) {
			(*tempI).SetXYPos(nextObjectX, nextObjectY);
			GetNextObjectCoordinates();
		}

		objectList.front().SetXYPos(nextObjectX, nextObjectY);
		GetNextObjectCoordinates();
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Calculates the coordinates of the next object to be placed in the inventory.
void Inventory::GetNextObjectCoordinates() {
	nextObjectX += 50 + objectList.front().GetWidth();
	if(nextObjectX >= 600) {
		nextObjectX = 120;
		nextObjectY += 150;
	}
}
//////////////////////////////////////////////////////////////////////////////////
Inventory* Inventory::GetInventory() {
	return this;
}
//////////////////////////////////////////////////////////////////////////////////
Ego* Inventory::GetEgo() { return pParent; }



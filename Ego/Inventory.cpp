#include "Inventory.h"
#include "Room.h"
#include "Framework\GraphicsDevice.h"

#define Error(x) MessageBox(NULL, x, "Error", MB_OK);

/// Default constructor.
Inventory::Inventory() { 
	SetInScript(false); 
	m_parser.SetParent(this);
	m_nextObjectX = 120; 
	m_nextObjectY = 30; }

/// Sets the parent of this inventory.  Must be called before a script is processed.
void Inventory::SetParent(Ego *p) { m_parent = p; }

/// Sets the tileset of this inventory, so that inventory objects can easily move from room to room.
void Inventory::SetInventoryTiles(GraphicsDevice *Graphics, long NumTextures) {
	m_inventoryTiles.Create(NumTextures);
}

/// Adds an object to the main inventory list.
void Inventory::AddInventoryObject(Object o) {
	if(o.GetName().find("Inventory") == string::npos) {
	stringstream s;
	s << o.GetName() << "Inventory";
	o.SetName(s.str());
	}
	o.SetHasOnStep(false);
	o.SetXYPos(m_nextObjectX, m_nextObjectY);
	float scale;
	if(o.GetHeight() > o.GetWidth()) {
		scale = (100.0f / (float)o.GetHeight());
	}
	else {
		scale = (100.0f / (float)o.GetWidth());
	}
	o.SetXScale(scale);
	o.SetYScale(scale);
	o.SetVisible(true);
	AddObject(o);
	GetNextObjectCoordinates(); // calculate the next coordinates
}


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
	for(iObject = m_objectList.begin(); iObject != m_objectList.end(); iObject++) {
		// Check if the mouse is hovering over an object.
		if((*iObject).CheckMouseCollision(mouseX, mouseY)) {
			// if so, we need to render the descriptor.
			if(m_parent->GetCurrentlyHeldItem() == 0) {
			SetCursor(LoadCursorFromFile("red-cursor.cur"));
			}
			m_curMouseObject = (*iObject).GetDescriptor();
			if(lClick) {
				SetCurActionObject(&(*iObject));
				if(m_parent->GetCurrentlyHeldItem() != 0) {	
					(*iObject).UseItem(m_parent->GetCurrentlyHeldItem()->GetName(), &m_parser); 
					SetInScript(true);
				}
				else if(GetGlobalAction() == IS_USE) { 
					stringstream cursorFile;
					cursorFile << (*iObject).GetName() << ".cur";
					SetCursor(LoadCursorFromFile((char*)cursorFile.str().c_str()));
					m_parent->SetCurrentlyHeldItem(&(*iObject));
				}
				else {
					// If the user also clicked on this object, then we need to process a script.
					(*iObject).DoAction(GetGlobalAction(), &m_parser);
					// We are now in script.
					SetInScript(true);
				}
				return;
			} // end if(lClick)
			return;
		} // end if((*iObject).CheckMouseCollision(mouseX, mouseY))
		m_curMouseObject = ""; // otherwise, there is no current object.
	} // end for
}

/// Updates a current script by executing the next line and processing input.
void Inventory::UpdateScript(long mouseX, long mouseY, bool lClick) {
	// if the parser has finished the line and is not waiting, execute the next line
	if(m_parser.FinishedLine() && !m_parser.WaitingForInput()) {
			m_parser.ExecuteNextLine();
		}
	// if the parser is waiting, then we are in a conversation.  
	else if(m_parser.WaitingForInput()) {
		std::list<TextBox>::iterator iText;
		for(iText = m_activeChoices.begin(); iText != m_activeChoices.end(); iText++) {
			// If the mouse is hovering, then we need to change the text color.
			if((*iText).CheckMouseCollision(mouseX, mouseY)) {
				// If the user chose a conversation choice, then we need to jump to that choice.
				(*iText).SetColor(0xFF532FD4);
				if(lClick) {
					m_parser.JumpToConversation((*iText).GetChoiceNumber());
					return;
				} // end if(lClick)
				return;
			} // end if
		} // end for
	} // end else if
}

/// Renders the inventory, including any objects in the inventory, and any script strings.
void Inventory::RenderInventory() {
	// First, render the objects.
	for(iObject = m_objectList.begin(); iObject != m_objectList.end(); iObject++) {
		(*iObject).Render();
	}
	// If we are not in a script, render the descriptor of the current mouse object, if any 
	if(!GetInScript()) {
		m_font->render(m_curMouseObject.c_str(), 0, 570, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFFFFFF, DT_CENTER);
	}
	// If we are in script, render the current conversation string, if any
	else if(GetInScript() && !m_parser.WaitingForInput()) {
		m_font->render(m_curConversationString.c_str(), 0, 250,
		650, SCREEN_HEIGHT, m_curConversationStringColor, DT_WORDBREAK | DT_CENTER);
	}
	// if the parser is waiting for input, then render any conversation choices.
	else if(GetInScript() && m_parser.WaitingForInput()) {
		PrintConversationChoices();
	}
	if(m_parent->GetCurrentlyHeldItem() != NULL) {
		m_font->render(m_parent->GetCurrentlyHeldItem()->GetName().c_str(), 0, 0, SCREEN_WIDTH,
					  SCREEN_HEIGHT, 0xFFFFFFFF);
	}
}

/// Finds an object in the inventory.
/** Because find object is used by Parser, and a script will almost positively have dialogue from Ego,
	the inventory must be able to return its ego parent.  If it is not asked for EGO, then it looks
	through the objectList.  If it is not an inventory item, then it looks through the room's items.
*/
Object* Inventory::FindObject(std::string objectName) {
	if(objectName.compare("EGO") == 0) { return m_parent; }
	for(iObject = m_objectList.begin(); iObject != m_objectList.end(); iObject++) {
		if((*iObject).GetName().compare(objectName) == 0) return &(*iObject);
	}
	return m_parent->GetRoom()->FindObject(objectName);
	return 0;
}

/// Removes an item from inventory, and retools coordinates.
void Inventory::LoseObject(std::string objectName) {
	bool found = false;
	std::list<Object>::iterator tempI;
	for(iObject = m_objectList.begin(); iObject != m_objectList.end(); iObject++) {
		if((*iObject).GetName().compare(objectName) == 0) {
			if(m_parent->GetCurrentlyHeldItem() != 0 && 
			  (*iObject).GetName().compare(m_parent->GetCurrentlyHeldItem()->GetName()) == 0) {
				m_parent->SetCurrentlyHeldItem(0);
			}
		m_nextObjectX = (*iObject).GetXPos();
		m_nextObjectY = (*iObject).GetYPos();
		tempI = iObject;
		// iterate before the object to be erased.
		if(tempI != m_objectList.begin()) {
			tempI--;
			m_objectList.erase(iObject);
			found = true;
			break;
		}
		else {
			m_objectList.erase(iObject);
			return;
		}
		}
	}
	if(found) {
		for(tempI; tempI != m_objectList.begin(); tempI--) {
			(*tempI).SetXYPos(m_nextObjectX, m_nextObjectY);
			GetNextObjectCoordinates();
		}
		m_objectList.front().SetXYPos(m_nextObjectX, m_nextObjectY);
		GetNextObjectCoordinates();
	}
}

	

/// Calculates the coordinates of the next object to be placed in the inventory.
void Inventory::GetNextObjectCoordinates() {
	m_nextObjectX += 50 + m_objectList.front().GetWidth();
	if(m_nextObjectX >= 600) {
		m_nextObjectX = 120;
		m_nextObjectY += 150;
	}
}

/// Returns itself.  Used by Parser.
Inventory* Inventory::GetInventory() {
	return this;
}

/// Returns Ego parent.  Used by Parser.
Ego* Inventory::GetEgo() { return m_parent; }



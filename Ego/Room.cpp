#include "Room.h"
#include "Framework\System.h"
#include "ScriptParser.h"
#include "DataPackage.h"

#define Error(x) MessageBox(NULL, x, "Error", MB_OK);

/// Default Constructor.
Room::Room() : m_dpCollision(NULL) { 	
	m_zeroScale = 1.0f;
	m_scalingFactor = 0.0f;
	m_parser->SetParent(this); 
}

Room::~Room() {
	delete m_dpCollision;
}

/// Sets the font and Ego of this room instance.
/** One room instance should be used for multiple rooms.
	Since the Ego character and font will not change per room,
	they are set separately from other room data.
*/
void Room::Set(Ego Dude, Font *font) {
	m_Ego = Dude;
	m_Ego.SetInventoryParent();
	m_Ego.SetParentRoom(this);
	m_font = font;
}

/// Enters the room.
/** \param cm The collision map for this room.
*/
void Room::EnterRoom(const DataPackage * dp_collision, std::string roomName) { 
	m_Ego.SetCurAction(IS_NOACTION, NULL, -1, -1);
	m_Ego.SetXScale(m_zeroScale - m_scalingFactor*(600 - m_Ego.GetYPos()));
	m_Ego.SetYScale(m_zeroScale - m_scalingFactor*(600 - m_Ego.GetYPos()));
	m_dpCollision = dp_collision;
	m_collisionMap = (char *)m_dpCollision->GetPtr();
	m_roomName = roomName;
	
	if(GetHasEnterScript()) {
		string s = roomName + "OnEnter.sc";
		m_parser->ParseFile(s);
		SetInScript(true);
	}
	else {
		SetInScript(false);
	}
}

/// Sets the status of this room's onEnter script.
void Room::SetHasEnterScript(bool d) {
	m_hasEnterScript = d;
}

/// Returns true if this room has an onEnter script.
bool Room::GetHasEnterScript() const { return m_hasEnterScript; }

/// Leaves the current room.
/** This function clears all data, and saves any room/object flags which were changed.
*/
bool Room::LeaveRoom() {
	DataPackage	m_DPVariables(sizeof(int)*(256 + 40*m_objectList.size())); ///< DataPackage to load/save all object and room flags.

	// create the data buffer.
	int* m_vars = (int *)m_DPVariables.GetPtr();
	
	// first, save the room flags.
	for(int i = 0; i<256; i++) {
		m_vars[i] = GetFlag(i);
	}

	// then, for each object, save the object flags.
	for(std::list<Object>::iterator iObject = m_objectList.begin(); iObject != m_objectList.end(); iObject++) {
		int k =0;
		int l = 1;
		for(int j = 40*l; j<40*(l+1); j++) {
			m_vars[j] = (*iObject).GetFlag(k);
			k++;
		}
		l++;
	
	}
	// save flags
	m_DPVariables.Save("Room.var");

	// free memory.
	m_objectList.clear();
	m_ExitList.clear();
	delete m_collisionMap;
	return true;
}

/// Returns a pointer to the Ego of this room instance.
Ego* Room::GetEgo() { return &m_Ego; }

/// Adds an exit to room rmNumber at location.
void Room::AddExit(RECT location, int rmNumber) {
	Exit temp(location, rmNumber);
	m_ExitList.push_front(temp);
}

/// Removes the first exit to room 'number' found.
/** \param number The number of the room to which the exit you wish to remove points.
	\return true if the exit is found and removed, false otherwise.
*/
bool Room::RemoveExit(int number) {
	bool found = false;
	for(iExit = m_ExitList.begin(); iExit != m_ExitList.end(); iExit++) {
		if((*iExit).GetRoomNumber() == number) {
			m_ExitList.erase(iExit);
			found = true;
			break;
		}
	}
	return found;
}

/// Relays input to the room for processing.
/** \param mouseX X-Coordinate of the mouse.
	\param mouseY Y-Coordinate of the mouse.
	\param lClick true if the left mouse button was clicked, false otherwise.
*/
void Room::QueryRoom(long mouseX, long mouseY, bool lClick) {
	long pathX = mouseX;
	long pathY = mouseY;
	m_curMouseObject = "";
	if(GetInScript()) { 
		if(m_parser->WaitingForInput()) {
			std::list<TextBox>::iterator iText;
			for(iText = m_activeChoices.begin(); iText != m_activeChoices.end(); iText++) {
				if((*iText).CheckMouseCollision(mouseX, mouseY)) {
					(*iText).SetColor(0xFF532FD4);
					if(lClick) { m_parser->JumpToConversation((*iText).GetChoiceNumber()); }
					return;
				} // end if((*iText...
			} // end for(iText...
		} // end if(m_parser->..
		return; // if we are in script and not waiting for input, we do not need to query the room
	}
	
	bool found = false;
	SetExit(false);
	
	// first, iterate through all of the objects.
	std::list<Object>::reverse_iterator	riObject;
	for(riObject = m_objectList.rbegin(); riObject != m_objectList.rend(); riObject++) {
		// if the object is visible...
		if((*riObject).GetIsVisible()) {
			// if the mouse is hovering over the object...
			if((*riObject).CheckMouseCollision(mouseX, mouseY)) {
				found = true;
				// set the current mouse object to the object's descriptor.
				m_curMouseObject = (*riObject).GetDescriptor();
				if(GetEgo()->GetCurrentlyHeldItem() == 0) {
				SetCursor(LoadCursorFromFile("red-cursor.cur"));
				}
				// if the mouse was clicked...
				if(lClick) {
					// Set the current object, 
					Object *temp = &(*riObject); 
					// and set ego moving toward that object with the current action.
					pathX = temp->GetXWalkCoordinate();
					pathY = temp->GetYWalkCoordinate();
					//m_Ego.SetEgoMoving(pathX, pathY);
					if(m_Ego.GetCurrentlyHeldItem() != 0) { 
						m_Ego.SetCurAction(IS_USEITEM, temp, pathX, pathY);
					}
					else {
					m_Ego.SetCurAction(m_curAction, temp, pathX, pathY);
					}
				} // end if(lClick)
				// we have found the active object/exit.
			} // end if((*riObject).CheckMouseCollision(...
			// also, check if Ego has stepped on an object that has an onStep script.
			if((*riObject).HasOnStep() && !(*riObject).GetEgoIn() && (*riObject).CheckMouseCollision(m_Ego.GetFootXPos(), m_Ego.GetFootYPos())) {
				(*riObject).SetEgoIn(true);
				(*riObject).DoAction(IS_WALK, m_parser);
				SetCurActionObject(&(*riObject));
				SetInScript(true);
				return;
			}
			// Finally, check if Ego has left an object.
			if((*riObject).GetEgoIn() && !(*riObject).CheckMouseCollision(m_Ego.GetFootXPos(), m_Ego.GetFootYPos())) {
				(*riObject).SetEgoIn(false);
			}
		} // end if((*riObject).GetIsVisible())
		if(found == true) { break; }
	} // end for

	// if we are not searching for an object...
	if(!found) {
		// iterate through the exits.
		for(iExit = m_ExitList.begin(); iExit != m_ExitList.end(); iExit++) {
			// if the mouse is hovering over an exit...
			if((*iExit).CheckMouseCollision(mouseX, mouseY)) {
				// Set the current mouse object to EXIT.
				m_curMouseObject = "EXIT";
				// if the mouse was clicked...
				if(lClick) {
					// Ego is trying to exit, SetExit.
					SetExit(true, (*iExit).GetRoomNumber());

					// Set Ego moving toward the exit.
					//m_Ego.SetEgoMoving(mouseX, mouseY);
					m_Ego.SetCurAction(IS_WALK, NULL, mouseX, mouseY);
					// we have found the object/exit.
					found = true;
					break;
				} // end if(lClick)
			} // end if((*iExit)...
		} // end for
			
		// otherwise, if the mouse was clicked and we still haven't found the object/exit...
		if(lClick && !found) {
			// Set ego walking toward the destination.
			//m_Ego.SetEgoMoving(mouseX, mouseY);
			m_Ego.SetCurAction(IS_WALK, NULL, mouseX, mouseY);
		} // end if(lClick...
	} // end if(!found)
	// if the mouse was clicked, find a path for Ego.
	if(lClick) {
		m_Ego.FindPath(pathX, pathY, m_collisionMap);
	}
}

/// Updates the room, and all objects within the room.
/** Update() is called each frame, and so is also used to execute the current script.
	\return false If Ego exits the room
*/
bool Room::Update() {
	// All object should be updated, regardless if we are in script.
	m_Ego.Update();
	m_Ego.SetXScale(m_zeroScale - m_scalingFactor*(600 - m_Ego.GetYPos()));
	m_Ego.SetYScale(m_zeroScale - m_scalingFactor*(600 - m_Ego.GetYPos()));
	// Update all objects in the scene
	for(std::list<Object>::iterator iObject = m_objectList.begin(); iObject != m_objectList.end(); iObject++) {
		(*iObject).Update();
	}
	// If we are not in script, then Ego is free to move/perform actions.
	if(!GetInScript())  {		
		// if Ego is ready to perform his current action...
		if(m_Ego.DoCurAction()) {
			// set the current object and action type.
			SetCurActionObject(m_Ego.GetCurActionObject());
			ActionType action = m_Ego.GetCurAction();
			// Reset Ego.
			m_Ego.SetCurAction(IS_NOACTION, NULL, -1, -1);
			// Load script.
			if(action != IS_USEITEM) {
			m_curActionObject->DoAction(action, m_parser);
			}
			else {
				m_curActionObject->UseItem(m_Ego.GetCurrentlyHeldItem()->GetName(), m_parser);
			}
			// Set script status to true.
			SetInScript(true);
		}
		// If Ego is going to exit, call LeaveRoom() and return false.
		if(m_Ego.AtCurActionCoord() && GetExit()) {
			LeaveRoom();
			return false;
		}
	}
	// if the room is currently executing a script, and the parser has
	// finished executin the previously line and is not waiting for input,
	// execute the next line.
	if(GetInScript() && m_parser->FinishedLine() && !m_parser->WaitingForInput()) {
			m_parser->ExecuteNextLine();
		}
	return true;
}

/// Returns true if Ego current action is to exit the room
bool Room::GetExit() const {
	return m_ActiveExit;
}

/// Sets Ego's current action as an exiting action, to room number number.
void Room::SetExit(bool active, int number) {
	m_ActiveExit = active;
	m_ActiveRoomNumber = number;
}

/// Returns the room number to which Ego is trying to exit (if any).
int Room::GetExitNum() const {
	return m_ActiveRoomNumber;
}

/// Renders the room, and all objects within.
/** This function also renders conversation choices, conversation strings and object descriptors.
*/
void Room::RenderRoom() const {
	// because of how room scripts are parsed, we must iterate backwards through the object list
	// (for clipping reasons).
	//for(list<Object>::reverse_iterator riObject = m_objectList.rbegin(); riObject != m_objectList.rend(); riObject++) {
	//	(*riObject).Render();
	//}
	for(std::list<Object>::const_iterator iObject = m_objectList.begin(); iObject != m_objectList.end(); iObject++) {
		(*iObject).Render();
	}

	m_Ego.Render();

	// If we are in script, render the current conversation string, if any.
	if(GetInScript()) {
		int textBoxWidth = 650;
		if(m_curConversationX + textBoxWidth > gSystem.getWidth() - 30) { 
			textBoxWidth = gSystem.getWidth() - m_curConversationX;
		}
		m_font->render(m_curConversationString.c_str(), m_curConversationX, m_curConversationY,
			textBoxWidth, gSystem.getHeight(), m_curConversationStringColor, DT_WORDBREAK);
	}

	// If the parser is waiting for input, render the current conversation choices.
	if(m_parser->WaitingForInput()) {
		PrintConversationChoices();
	}

	// If we are not in script, render the current object descriptor.
	if(!GetInScript()) {
		m_font->render(m_curMouseObject.c_str(), 400, 500, gSystem.getWidth(), gSystem.getHeight(), 0xFFFFFFFF);

		// render Ego's current action.
		std::string action;
		switch(m_curAction) {
			case IS_USE:
				action = "Use";
				break;
			case IS_WALK:
				action = "Walk";
				break;
			case IS_LOOK:
				action = "Look";
				break;
			case IS_TALK:
				action = "Talk";
				break;
			default:
				action = "Null";
				break;
	}
	m_font->render(action.c_str(), 10, 530, gSystem.getWidth(), gSystem.getHeight(), 0xDDDDDDDD);
	}
	if(m_Ego.GetCurrentlyHeldItem() != NULL) {
		m_font->render(m_Ego.GetCurrentlyHeldItem()->GetName().c_str(), 0, 0, gSystem.getWidth(), gSystem.getHeight(), 0xFFFFFFFF);
	}
}


/// Return a pointer to an object within the room, null if no object is found.
/** \param objectName The name of the object to search for.  If "EGO" is passed,
	FindObject() will return a pointer to Ego.
*/
Object* Room::FindObject(const std::string & objectName) {
	if(objectName.compare("EGO") == 0) { return &m_Ego; }

	for(std::list<Object>::iterator iObject = m_objectList.begin(); iObject != m_objectList.end(); iObject++) {
		if((*iObject).GetName().compare(objectName) == 0) { return &(*iObject); }
	}
	return 0;
}

/// Return the main inventory (necessary for parsing).
Inventory* Room::GetInventory() {
	return m_Ego.GetInventory();
}

/// Sets the scaling parameters of this room.
void Room::SetScaling(float zeroScale, float scalingFactor) {
	m_zeroScale = zeroScale;
	m_scalingFactor = scalingFactor;
}
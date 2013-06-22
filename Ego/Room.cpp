#include "Room.h"
#include "Framework\System.h"
#include "ScriptParser.h"
#include "DataPackage.h"
//////////////////////////////////////////////////////////////////////////////////
Room::Room() : dpCollision(NULL) { 	
	zeroScale = 1.0f;
	scalingFactor = 0.0f;
}
//////////////////////////////////////////////////////////////////////////////////
Room::~Room() {
	delete dpCollision;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the font and pEgo of this room instance.
/** One room instance should be used for multiple rooms.
	Since the pEgo character and font will not change per room,
	they are set separately from other room data.
*/
void Room::Init(Ego * pego, const Font *font) {
	pEgo = pego;
	pEgo->SetParentRoom(this);
	pFont = font;
}
//////////////////////////////////////////////////////////////////////////////////
/// Enters the room.
/** \param cm The collision map for this room.
*/
void Room::EnterRoom(const DataPackage * dp_collision, const std::string & roomName) { 
	this->roomName = roomName;
	
	dpCollision = dp_collision;
	collisionMap = (char *)dpCollision->GetPtr();
	
	pEgo->SetCurAction(IS_NOACTION, NULL, -1, -1);
	pEgo->SetXScale(zeroScale - scalingFactor*(600 - pEgo->GetYPos()));
	pEgo->SetYScale(zeroScale - scalingFactor*(600 - pEgo->GetYPos()));
		
	if(GetHasEnterScript()) {
		string s = roomName + "OnEnter.sc";
		pParser->ParseFile(s);
		SetInScript(true);
	} else {
		SetInScript(false);
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the status of this room's onEnter script.
void Room::SetHasEnterScript(bool d) {
	hasEnterScript = d;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns true if this room has an onEnter script.
bool Room::GetHasEnterScript() const { 
	return hasEnterScript; 
}
//////////////////////////////////////////////////////////////////////////////////
/// Leaves the current room.
/** This function clears all data, and saves any room/object flags which were changed.
*/
bool Room::LeaveRoom() {
	DataPackage	dp_variables(sizeof(int)*(256 + 40*objectList.size())); ///< DataPackage to load/save all object and room flags.

	// create the data buffer.
	int* vars = (int *)dp_variables.GetPtr();
	
	// first, save the room flags.
	for(int i = 0; i<256; i++) {
		vars[i] = GetFlag(i);
	}

	// then, for each object, save the object flags.
	for(std::list<Object>::iterator iObject = objectList.begin(); iObject != objectList.end(); iObject++) {
		int k =0;
		int l = 1;
		for(int j = 40*l; j<40*(l+1); j++) {
			vars[j] = (*iObject).GetFlag(k);
			k++;
		}
		l++;
	
	}
	// save flags
	dp_variables.Save("Room.var");

	// free memory.
	objectList.clear();
	exitList.clear();
	delete collisionMap;

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns a pointer to the pEgo of this room instance.
Ego* Room::GetEgo() { return pEgo; }
//////////////////////////////////////////////////////////////////////////////////
/// Adds an exit to room rmNumber at location.
void Room::AddExit(const RECT & location, int rmNumber) {
	Exit temp(location, rmNumber);
	exitList.push_front(temp);
}
//////////////////////////////////////////////////////////////////////////////////
/// Removes the first exit to room 'number' found.
/** \param number The number of the room to which the exit you wish to remove points.
	\return true if the exit is found and removed, false otherwise.
*/
bool Room::RemoveExit(int number) {
	bool found = false;
	for(std::list<Exit>::iterator iExit = exitList.begin(); iExit != exitList.end(); iExit++) {
		if((*iExit).GetRoomNumber() == number) {
			exitList.erase(iExit);
			found = true;
			break;
		}
	}
	return found;
}
//////////////////////////////////////////////////////////////////////////////////
/// Relays input to the room for processing.
/** \param mouseX X-Coordinate of the mouse.
	\param mouseY Y-Coordinate of the mouse.
	\param lClick true if the left mouse button was clicked, false otherwise.
*/
void Room::QueryRoom(long mouseX, long mouseY, bool lClick) {
	curMouseObject.clear();

	if(GetInScript()) { 
		if(pParser->WaitingForInput()) {
			for(std::list<TextBox>::iterator iText = activeChoices.begin(); iText != activeChoices.end(); iText++) {
				if((*iText).CheckMouseCollision(mouseX, mouseY)) {
					(*iText).SetColor(0xFF532FD4);
					if(lClick) { pParser->JumpToConversation((*iText).GetChoiceNumber()); }
					return;
				}
			}
		}
		
		return; // if we are in script and not waiting for input, we do not need to query the room
	}
	
	long pathX = mouseX;
	long pathY = mouseY;
	bool found = false;

	SetExit(false);
	
	// first, iterate through all of the objects.
	for(std::list<Object>::reverse_iterator riObject = objectList.rbegin(); riObject != objectList.rend(); riObject++) {
		// if the object is visible...
		if((*riObject).GetIsVisible()) {
			Object & checkedObject = (*riObject); 
			// if the mouse is hovering over the object...
			if(checkedObject.CheckMouseCollision(mouseX, mouseY)) {
				found = true;
				// set the current mouse object to the object's descriptor.
				curMouseObject = (*riObject).GetDescriptor();
				if(GetEgo()->GetCurrentlyHeldItem() == 0) {
					SetCursor(LoadCursorFromFile("red-cursor.cur"));
				}

				// if the mouse was clicked...
				if(lClick) {
					pathX = checkedObject.GetXWalkCoordinate();
					pathY = checkedObject.GetYWalkCoordinate();

					if(pEgo->GetCurrentlyHeldItem() != 0) { 
						pEgo->SetCurAction(IS_USEITEM, &checkedObject, pathX, pathY);
					} else {
						pEgo->SetCurAction(curAction, &checkedObject, pathX, pathY);
					}
				}
			}
			
			// also, check if pEgo has stepped on an object that has an onStep script.
			if(checkedObject.HasOnStep() && !checkedObject.GetEgoIn() && checkedObject.CheckMouseCollision(pEgo->GetFootXPos(), pEgo->GetFootYPos())) {
				checkedObject.SetEgoIn(true);
				checkedObject.DoAction(IS_WALK, pParser);
				SetCurActionObject(&checkedObject);
				SetInScript(true);
				return;
			}
			// Finally, check if pEgo has left an object.
			if(checkedObject.GetEgoIn() && !checkedObject.CheckMouseCollision(pEgo->GetFootXPos(), pEgo->GetFootYPos())) {
				checkedObject.SetEgoIn(false);
			}
		}
		
		if(found == true) { 
			break; 
		}
	}

	// if we are not searching for an object...
	if(!found) {
		// iterate through the exits.
		for(std::list<Exit>::iterator iExit = exitList.begin(); iExit != exitList.end(); iExit++) {
			// if the mouse is hovering over an exit...
			if((*iExit).CheckMouseCollision(mouseX, mouseY)) {
				// Set the current mouse object to EXIT.
				curMouseObject = "EXIT";
				// if the mouse was clicked...
				if(lClick) {
					// pEgo is trying to exit, SetExit.
					SetExit(true, (*iExit).GetRoomNumber());

					// Set pEgo moving toward the exit.
					//pEgo->SetpEgoMoving(mouseX, mouseY);
					pEgo->SetCurAction(IS_WALK, NULL, mouseX, mouseY);
					// we have found the object/exit.
					found = true;
					break;
				}
			}
		}
			
		// otherwise, if the mouse was clicked and we still haven't found the object/exit...
		if(lClick) {
			pEgo->SetCurAction(IS_WALK, NULL, mouseX, mouseY);
		}
	}
	
	// if the mouse was clicked, find a path for pEgo->
	if(lClick) {
		pEgo->FindPath(pathX, pathY, collisionMap);
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Updates the room, and all objects within the room.
/** Update() is called each frame, and so is also used to execute the current script.
	\return false If pEgo exits the room
*/
bool Room::Update() {
	// All object should be updated, regardless if we are in script.
	pEgo->Update();
	pEgo->SetXScale(zeroScale - scalingFactor*(600 - pEgo->GetYPos()));
	pEgo->SetYScale(zeroScale - scalingFactor*(600 - pEgo->GetYPos()));

	// Update all objects in the scene
	for(std::list<Object>::iterator iObject = objectList.begin(); iObject != objectList.end(); iObject++) {
		(*iObject).Update();
	}

	// If we are not in script, then pEgo is free to move/perform actions.
	if(!GetInScript())  {		
		// if pEgo is ready to perform his current action...
		if(pEgo->DoCurAction()) {
			// set the current object and action type.
			SetCurActionObject(pEgo->GetCurActionObject());
			ActionType action = pEgo->GetCurAction();
			pEgo->SetCurAction(IS_NOACTION, NULL, -1, -1);
			
			// Load script.
			if(action != IS_USEITEM) {
				curActionObject->DoAction(action, pParser);
			} else {
				curActionObject->UseItem(pEgo->GetCurrentlyHeldItem()->GetName(), pParser);
			}
			// Set script status to true.
			SetInScript(true);
		}

		// If pEgo is going to exit, call LeaveRoom() and return false.
		if(pEgo->AtCurActionCoord() && GetExit()) {
			LeaveRoom();
			return false;
		}
	}

	// if the room is currently executing a script, and the parser has
	// finished executin the previously line and is not waiting for input,
	// execute the next line.
	if(GetInScript() && pParser->FinishedLine() && !pParser->WaitingForInput()) {
		pParser->ExecuteNextLine();
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns true if pEgo current action is to exit the room
bool Room::GetExit() const {
	return activeExit;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets pEgo's current action as an exiting action, to room number number.
void Room::SetExit(bool active, int number) {
	activeExit = active;
	activeRoomNumber = number;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the room number to which pEgo is trying to exit (if any).
int Room::GetExitNum() const {
	return activeRoomNumber;
}
//////////////////////////////////////////////////////////////////////////////////
/// Renders the room, and all objects within.
/** This function also renders conversation choices, conversation strings and object descriptors.
*/
void Room::RenderRoom() const {
	// because of how room scripts are parsed, we must iterate backwards through the object list
	// (for clipping reasons).
	//for(list<Object>::reverse_iterator riObject = objectList.rbegin(); riObject != objectList.rend(); riObject++) {
	//	(*riObject).Render();
	//}
	for(std::list<Object>::const_iterator iObject = objectList.begin(); iObject != objectList.end(); iObject++) {
		(*iObject).Render();
	}

	pEgo->Render();

	// If we are in script, render the current conversation string, if any.
	if(GetInScript()) {
		int textBoxWidth = 650;
		if(curConversationX + textBoxWidth > gSystem.getWidth() - 30) { 
			textBoxWidth = gSystem.getWidth() - curConversationX;
		}
		
		pFont->render(curConversationString.c_str(), curConversationX, curConversationY,
			textBoxWidth, gSystem.getHeight(), curConversationStringColor, DT_WORDBREAK);
	}

	// If the parser is waiting for input, render the current conversation choices.
	if(pParser->WaitingForInput()) {
		PrintConversationChoices();
	}

	// If we are not in script, render the current object descriptor.
	if(!GetInScript()) {
		pFont->render(curMouseObject.c_str(), 400, 500, gSystem.getWidth(), gSystem.getHeight(), 0xFFFFFFFF);

		// render pEgo's current action.
		std::string action;
		switch(curAction) {
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
	
		pFont->render(action.c_str(), 10, 530, gSystem.getWidth(), gSystem.getHeight(), 0xDDDDDDDD);
	}

	if(pEgo->GetCurrentlyHeldItem() != NULL) {
		pFont->render(pEgo->GetCurrentlyHeldItem()->GetName().c_str(), 0, 0, gSystem.getWidth(), gSystem.getHeight(), 0xFFFFFFFF);
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Return a pointer to an object within the room, null if no object is found.
/** \param objectName The name of the object to search for.  If "pEgo" is passed,
	FindObject() will return a pointer to pEgo->
*/
Object* Room::FindObject(const std::string & objectName) {
	if(objectName.compare("Ego") == 0) { return pEgo; }

	for(std::list<Object>::iterator iObject = objectList.begin(); iObject != objectList.end(); iObject++) {
		if((*iObject).GetName().compare(objectName) == 0) { return &(*iObject); }
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the scaling parameters of this room.
void Room::SetScaling(float zeroScale, float scalingFactor) {
	zeroScale = zeroScale;
	scalingFactor = scalingFactor;
}
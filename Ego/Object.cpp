#include "Object.h"
#include "ScriptParser.h"
#include <sstream>
//////////////////////////////////////////////////////////////////////////////////
Object::Object() {
	for(int i=0; i<40; i++) {
		scriptFlags[i] = 1;
	}

	hasOnStepScript = true;
	isEgoIn = false;
	useItemsOn.clear();
	curTalkAnimation = -1;
}
//////////////////////////////////////////////////////////////////////////////////
Object::~Object() { }
//////////////////////////////////////////////////////////////////////////////////
/// Begins a script.
/** DoAction determines which script to process, based upon the action done to it.
	\param action The ActionType performed on this object.
	\param p The parser which will parse this object's script.
*/
bool Object::DoAction(ActionType action, Parser *p) {
	std::ostringstream scriptFile;
	scriptFile << "Data\\Scripts\\";
	switch(action) {
		case IS_USE:
			scriptFile << GetName() << "_Use.sc";
			break;
		case IS_TALK:
			scriptFile << GetName() << "_Talk.sc";
			break;
		case IS_LOOK:
			scriptFile << GetName() << "_Look.sc";
			break;
		case IS_WALK:
			scriptFile << GetName() << "_onStep.sc";
			break;
	}
	
	return p->ParseFile(scriptFile.str());
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the text color of this object.
void Object::SetTextColor(D3DCOLOR color) { textColor = color; }
//////////////////////////////////////////////////////////////////////////////////
/// Returns the text color of this object.
Color Object::GetTextColor() const { return textColor; }
//////////////////////////////////////////////////////////////////////////////////
/// Returns the status of the flag at index.
int Object::GetFlag(int index) const { return scriptFlags[index]; }
//////////////////////////////////////////////////////////////////////////////////
/// Sets the status of the flag at index to d.
void Object::SetFlag(int index, int d) { scriptFlags[index] = d; }
//////////////////////////////////////////////////////////////////////////////////
/// Sets the descriptor of this object.
void Object::SetDescriptor(const std::string & descriptor) { strDescriptor = descriptor; }
//////////////////////////////////////////////////////////////////////////////////
/// Returns the descriptor of this object.
const std::string & Object::GetDescriptor() const { return strDescriptor; }
//////////////////////////////////////////////////////////////////////////////////
/// Sets if Ego is in this object.
void Object::SetEgoIn(bool d) { isEgoIn = d; }
//////////////////////////////////////////////////////////////////////////////////
///  True if Ego is in this object.
bool Object::GetEgoIn() const { return isEgoIn; }
//////////////////////////////////////////////////////////////////////////////////
/// Sets if this object executes a script when stepped upon.
bool Object::HasOnStep() const { return hasOnStepScript; }
//////////////////////////////////////////////////////////////////////////////////
/// True if this object executes a script when stepped upon.
void Object::SetHasOnStep(bool d) { hasOnStepScript = d; }
//////////////////////////////////////////////////////////////////////////////////
/// Set the coordinates that actors walk to when moving to this object.
void Object::SetXYWalkCoordinates(long x, long y) { walkCoordinates.x = x; walkCoordinates.y = y; }
//////////////////////////////////////////////////////////////////////////////////
/// Return the x walk-coordinate.
long Object::GetXWalkCoordinate() const { return walkCoordinates.x; }
//////////////////////////////////////////////////////////////////////////////////
/// Return the y-walk coordinate.
long Object::GetYWalkCoordinate() const { return walkCoordinates.y; }
//////////////////////////////////////////////////////////////////////////////////
/// Adds an object that has a special script when used upon this object.
/** \param itemName Name of the special item. */
void Object::AddUseItem(const std::string & itemName) { 
	useItemsOn.push_back(itemName);
}
//////////////////////////////////////////////////////////////////////////////////
/// Scans through the objects usable on this object and processes the correct script.
bool Object::UseItem(const std::string & item, Parser *p) {
	// If no item, is found, we will process the generic script.
	std::stringstream scriptFile;
	scriptFile << "Data\\Scripts\\" << item << "Generic_UseItem.sc";
	for(std::list<std::string>::iterator iString = useItemsOn.begin(); iString != useItemsOn.end(); iString++) {
		if(item.compare((*iString)) == 0) {
			// if the item is found, process the correct script.
			scriptFile.str("");
			scriptFile << "Data\\Scripts\\" << item << "On" << GetName() << "_UseItem.sc";
		}
	}
	
	return p->ParseFile(scriptFile.str());
}
//////////////////////////////////////////////////////////////////////////////////
/// Executes the current talking animation of this object (or does nothing if this object has no talking animation).
void Object::DoTalkingAnimation() {
	if(curTalkAnimation != -1) {
	SetAnimation(curTalkAnimation);
	SetAutoAnimate(true);
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the current talking animation.
void Object::SetCurTalkingAnimation(int animationNumber) { curTalkAnimation = animationNumber; }
//////////////////////////////////////////////////////////////////////////////////
/// Returns the number of the current talking animation.
int Object::GetCurTalkingAnimation() const { return curTalkAnimation; }
//////////////////////////////////////////////////////////////////////////////////		
/// Checks if this Sprite's bounding rectangle collides with RECT a.
bool Object::CheckCollisions(const RECT & a) const {
	RECT rcIntersect;
	IntersectRect(&rcIntersect, &(GetRect()), &a);
	if(IsRectEmpty(&rcIntersect) != 0) {
		return false; 
	} else { 
		return true; 
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Checks if the coordinates MouseX and MouseY are within the Sprite's bounding rectangle.
bool Object::CheckMouseCollision(long MouseX, long MouseY) const {
	if(MouseX < m_Location.x) return false;
	if(MouseX >= m_Location.x + m_Width) return false;
	if(MouseY < m_Location.y) return false;
	if(MouseY >= m_Location.y + m_Height) return false;

	return true;
}
#include "Object.h"
#include "ScriptParser.h"
#include <sstream>
#define Error(x) MessageBox(NULL, x, "Error", MB_OK);

/// Default constructor.  Resets all script flags to 1 (on).
Object::Object() {
	for(int i=0; i<40; i++) {
		m_flags[i] = 1;
	}
	m_hasOnStepScript = true;
	m_isEgoIn = false;
	m_useItemsOn.clear();
	m_curTalkAnimation = -1;
}

/// Default destructor.
Object::~Object() { }

/// Begins a script.
/** DoAction determines which script to process, based upon the action done to it.
	\param action The ActionType performed on this object.
	\param p The parser which will parse this object's script.
*/
void Object::DoAction(ActionType action, Parser *p) {
	std::ostringstream scriptFile;
	scriptFile << "Scripts\\";
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
	p->ParseFile(scriptFile.str());

}

/// Sets the text color of this object.
void Object::SetTextColor(D3DCOLOR color) { m_textColor = color; }

/// Returns the text color of this object.
D3DCOLOR Object::GetTextColor() { return m_textColor; }

/// Returns the status of the flag at index.
int Object::GetFlag(int index) { return m_flags[index]; }

/// Sets the status of the flag at index to d.
void Object::SetFlag(int index, int d) { m_flags[index] = d; }

/// Sets the descriptor of this object.
void Object::SetDescriptor(std::string descriptor) { m_descriptor = descriptor; }

/// Returns the descriptor of this object.
std::string Object::GetDescriptor() { return m_descriptor; }

/// Sets if Ego is in this object.
void Object::SetEgoIn(bool d) { m_isEgoIn = d; }

///  True if Ego is in this object.
bool Object::GetEgoIn() { return m_isEgoIn; }

/// Sets if this object executes a script when stepped upon.
bool Object::HasOnStep() { return m_hasOnStepScript; }

/// True if this object executes a script when stepped upon.
void Object::SetHasOnStep(bool d) { m_hasOnStepScript = d; }

/// Set the coordinates that actors walk to when moving to this object.
void Object::SetXYWalkCoordinates(long x, long y) { m_walkCoordinates.x = x; m_walkCoordinates.y = y; }

/// Return the x walk-coordinate.
long Object::GetXWalkCoordinate() { return m_walkCoordinates.x; }

/// Return the y-walk coordinate.
long Object::GetYWalkCoordinate() { return m_walkCoordinates.y; }

/// Adds an object that has a special script when used upon this object.
/** \param itemName Name of the special item. */
void Object::AddUseItem(std::string itemName) { 
	m_useItemsOn.push_back(itemName);
}

/// Scans through the objects usable on this object and processes the correct script.
void Object::UseItem(std::string item, Parser *p) {
	// If no item, is found, we will process the generic script.
	std::stringstream scriptFile;
	scriptFile << "Scripts\\" << item << "Generic_UseItem.sc";
	for(std::list<std::string>::iterator iString = m_useItemsOn.begin(); iString != m_useItemsOn.end(); iString++) {
		if(item.compare((*iString)) == 0) {
			// if the item is found, process the correct script.
			scriptFile.str("");
			scriptFile << "Scripts\\" << item << "On" << GetName() << "_UseItem.sc";
		}
	}
	p->ParseFile(scriptFile.str());
}

/// Executes the current talking animation of this object (or does nothing if this object has no talking animation).
void Object::DoTalkingAnimation() {
	if(m_curTalkAnimation != -1) {
	SetAnimation(m_curTalkAnimation);
	SetAutoAnimate(true);
	}
}

/// Sets the current talking animation.
void Object::SetCurTalkingAnimation(int animationNumber) { m_curTalkAnimation = animationNumber; }

/// Returns the number of the current talking animation.
int Object::GetCurTalkingAnimation() { return m_curTalkAnimation; }
			
/// Checks if this Sprite's bounding rectangle collides with RECT a.
bool Object::CheckCollisions(RECT a) {
	RECT rcIntersect;
	IntersectRect(&rcIntersect, &(GetRect()), &a);
	if(IsRectEmpty(&rcIntersect) != 0) {
		

		return false; }
	else { 
		return true; }
}

/// Checks if the coordinates MouseX and MouseY are within the Sprite's bounding rectangle.
bool Object::CheckMouseCollision(long MouseX, long MouseY) {
	if(MouseX < m_Location.x) return false;
	if(MouseX >= m_Location.x + m_Width) return false;
	if(MouseY < m_Location.y) return false;
	if(MouseY >= m_Location.y + m_Height) return false;
	return true;
}
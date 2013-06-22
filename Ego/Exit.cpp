#include "Exit.h"
//////////////////////////////////////////////////////////////////////////////////
/// Constructor to set the location of the exit and the room number to which it exits.
/** \param location Rectangle defining the location of that exit in its Room.
	\param rmNumber the room to which this exits connects.
*/
Exit::Exit(const RECT & location, int rmNumber) {
	roomRect = location;
	roomNumber = rmNumber;
}
//////////////////////////////////////////////////////////////////////////////////
/// Getter for the exit's location rectangle.
const RECT & Exit::GetRect() const { return roomRect; }
//////////////////////////////////////////////////////////////////////////////////
/// Getter for the exit's room number.
int	Exit::GetRoomNumber() const { return roomNumber; }
//////////////////////////////////////////////////////////////////////////////////
/// Check a point collision with the exit's location rectangle
bool Exit::CheckMouseCollision(long mouseX, long mouseY) const {
	if(mouseX < roomRect.left) return false;
	if(mouseX >= roomRect.right) return false;
	if(mouseY < roomRect.top) return false;
	if(mouseY >= roomRect.bottom) return false;
	return true;
}
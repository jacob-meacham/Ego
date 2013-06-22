#include "Exit.h"
#define Error(x) MessageBox(NULL, x, "Error", MB_OK);

/// Constructor to set the location of the exit and the room number to which it exits.
/** \param location Rectangle defining the location of that exit in its Room.
	\param rmNumber the room to which this exits connects.
*/
Exit::Exit(RECT location, int rmNumber) {
	m_rect = location;
	m_roomNumber = rmNumber;
}

/// Getter for the exit's location rectangle.
RECT Exit::GetRect() const { return m_rect; }

/// Getter for the exit's room number.
int	Exit::GetRoomNumber() const { return m_roomNumber; }

/// Check a point collision with the exit's location rectangle
bool Exit::CheckMouseCollision(long mouseX, long mouseY) const {
	if(mouseX < m_rect.left) return false;
	if(mouseX >= m_rect.right) return false;
	if(mouseY < m_rect.top) return false;
	if(mouseY >= m_rect.bottom) return false;
	return true;
}
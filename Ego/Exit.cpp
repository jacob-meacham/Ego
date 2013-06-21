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

/// Setter for the exit's location rectangle.
void Exit::SetRect(RECT r) {
	m_rect = r;
}

/// Getter for the exit's location rectangle.
RECT Exit::GetRect() { return m_rect; }

/// Setter for the exit's room number.
void Exit::SetRoomNumber(int r) { m_roomNumber = r; }

/// Getter for the exit's room number.
int	Exit::GetRoomNumber() { return m_roomNumber; }

/// Check a point collision with the exit's location rectangle
bool Exit::CheckMouseCollision(long mouseX, long mouseY) { 
	if(mouseX < m_rect.left) return false;
	if(mouseX >= m_rect.right) return false;
	if(mouseY < m_rect.top) return false;
	if(mouseY >= m_rect.bottom) return false;
	return true;
}
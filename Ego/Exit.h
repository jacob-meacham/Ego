#pragma once
#include <windows.h>

/// Encapsulates exits from any room.
class Exit {
	private:
		int		m_roomNumber; ///< Room number to which this exit leads.
		RECT	m_rect; ///< Rectangle that defines this exit's posiation.
	public:
		// Constructor.
		Exit(RECT location, int rmNumber);
		bool CheckMouseCollision(long mouseX, long mouseY) const;

		// Getters.
		int	GetRoomNumber() const;
		RECT GetRect() const;
};
#pragma once
#include <windows.h>

/// Encapsulates exits from any room.
class Exit {
	private:
		int		roomNumber; ///< Room number to which this exit leads.
		RECT	roomRect; ///< Rectangle that defines this exit's position.
	public:
		// Constructor.
		Exit(const RECT & location, int rmNumber);
		bool CheckMouseCollision(long mouseX, long mouseY) const;

		// Getters.
		int	GetRoomNumber() const;
		const RECT & GetRect() const;
};
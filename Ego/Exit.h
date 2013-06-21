#ifndef _exit_h_included_
#define _exit_h_included_
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
/// Encapsulates exits from any room.
class Exit {
	private:
		int		m_roomNumber; ///< Room number to which this exit leads.
		RECT	m_rect; ///< Rectangle that defines this exit's posiation.
	public:
		// Constructor.
		Exit(RECT location, int rmNumber);

		bool CheckMouseCollision(long mouseX, long mouseY);

		// Setters.
		void SetRoomNumber(int r);
		void SetRect(RECT r);

		// Getters.
		int	GetRoomNumber();
		RECT GetRect();
};
#endif
#ifndef _textBox_h_included_
#define _textBox_h_included_
#include <string>
#include "Graphics_Core.h"

/// Encapsulates a text box, used for rendering conversation choices.
/** The textbox is primarily used by GameArea to render the active conversation choices.
	TextBox gives access to collision functions, and automatically determines the height of the text box.
*/
class TextBox {
private:
	Font			*p_Font; ///< Parent font of this textbox.
	std::string		m_Text; ///< The text contained in this textbox.
	D3DCOLOR		m_TextColor; ///< The color of the text in this textbox.
	long			m_XPos, m_YPos; ///< The upper-left coordinates of the text-box.
	long			m_Width, m_Height; ///< The width and height of the textbox.
	int				m_choiceNumber; ///< Number ID of this textbox.

public:
	// Creates a new text box, setting the parent font.  
	void Create(Font *parent);

	// Sets the text in the text box.
	bool SetText(int number, std::string text, D3DCOLOR textColor, long xPos, long yPos, long Width);

	// Prints the text box.
	void PrintTextBox();

	// Returns true if the mouse is hovering over this text.
	bool CheckMouseCollision(long mouseX, long mouseY);

	// Setter.
	void SetColor(D3DCOLOR textColor);

	// Getters.
	int GetChoiceNumber();
	long GetHeight();

};
#endif
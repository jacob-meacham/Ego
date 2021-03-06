#pragma once
#include <string>
#include "Framework\Font.h"

/// Encapsulates a text box, used for rendering conversation choices.
/** The textbox is primarily used by GameArea to render the active conversation choices.
	TextBox gives access to collision functions, and automatically determines the height of the text box.
*/
class TextBox {
private:
	const Font * pFont; ///< Parent font of this textbox.
	std::string	strText; ///< The text contained in this textbox.
	D3DCOLOR textColor; ///< The color of the text in this textbox.
	long xPos, yPos; ///< The upper-left coordinates of the text-box.
	long width, height; ///< The width and height of the textbox.
	int	choiceNumber; ///< Number ID of this textbox.

public:
	TextBox(const Font * pfont);

	// Sets the text in the text box.
	bool SetText(int number, const std::string & text, D3DCOLOR textColor, long xPos, long yPos, long width);

	// Prints the text box.
	void PrintTextBox() const;

	// Returns true if the mouse is hovering over this text.
	bool CheckMouseCollision(long mouseX, long mouseY) const;

	// Setter.
	void SetColor(D3DCOLOR textColor);

	// Getters.
	int GetChoiceNumber() const;
	long GetHeight() const;

};
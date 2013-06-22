#include "TextBox.h"
//////////////////////////////////////////////////////////////////////////////////
TextBox::TextBox(const Font * pfont) : pFont(pfont) { 

}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the text in the text box.
/** This function calculates the height of the text box, based on the text contained therein.
	\param number The ID of this text box.
	\param Width Fixes width, and allows for a variable height.
*/
bool TextBox::SetText(int number, const std::string & text, D3DCOLOR textColor, long xPos, long yPos, long width) {
	if(!pFont) { return false; }
	
	choiceNumber = number;
	strText = text;
	this->textColor = textColor;
	this->yPos = yPos;
	this->xPos = xPos;
	this->width = width;

	// Create rectangle used to determine textbox height
	RECT r;
	r.left = xPos;
	r.top = 0;
	r.right = xPos + width - 12;
	r.bottom = 1;

	// Determine height, using the DirectX function DrawText().
	height = pFont->calcHeight(text.c_str(), xPos, 0, width-12, 1) + 12;
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the color of the text.
void TextBox::SetColor(D3DCOLOR textColor) { this->textColor = textColor; }
//////////////////////////////////////////////////////////////////////////////////
/// Returns true if the mouse is hovering over this text.
bool TextBox::CheckMouseCollision(long mouseX, long mouseY) const {
	if(mouseX < xPos) return false;
	if(mouseX >= xPos + width) return false;
	if(mouseY < yPos) return false;
	if(mouseY >= yPos + height) return false;
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Prints the text box.
void TextBox::PrintTextBox() const {
	pFont->render(strText.c_str(), xPos+6, yPos+6, width-12, height-12, textColor, DT_WORDBREAK);
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the ID number of the text box.
int TextBox::GetChoiceNumber() const {
	return choiceNumber;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the height (width is fixed).
long TextBox::GetHeight() const {
	return height;
}
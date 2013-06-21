#include "TextBox.h"

/// Creates a new text box, setting the parent font.  
/** This function must be called before SetText(). */
void TextBox::Create(Font *parent) {
	p_Font = parent;
}

/// Sets the text in the text box.
/** This function calculates the height of the text box, based on the text contained therein.
	\param number The ID of this text box.
	\param Width Fixes width, and allows for a variable height.
*/
bool TextBox::SetText(int number, std::string text, D3DCOLOR textColor, long xPos, long yPos, long Width) {
	if(p_Font == NULL) { return false; }
	
	m_choiceNumber = number;
	m_Text = text;
	m_TextColor = textColor;
	m_YPos = yPos;
	m_XPos = xPos;
	m_Width = Width;

	// Create rectangle used to determine textbox height
	RECT r;
	r.left = xPos;
	r.top = 0;
	r.right = xPos + Width - 12;
	r.bottom = 1;

	// Determine height, using the DirectX function DrawText().
	m_Height = p_Font->GetFontCOM()->DrawText((char*)text.c_str(), -1, &r, DT_CALCRECT | DT_WORDBREAK, 0xFFFFFFFF) + 12;
	return true;
}

/// Sets the color of the text.
void TextBox::SetColor(D3DCOLOR textColor) { m_TextColor = textColor; }

/// Returns true if the mouse is hovering over this text.
bool TextBox::CheckMouseCollision(long mouseX, long mouseY) {
	if(mouseX < m_XPos) return false;
	if(mouseX >= m_XPos + m_Width) return false;
	if(mouseY < m_YPos) return false;
	if(mouseY >= m_YPos + m_Height) return false;
	return true;
}

/// Prints the text box.
void TextBox::PrintTextBox() {
	p_Font->Print((char*)m_Text.c_str(), m_XPos+6, m_YPos+6, m_Width-12, m_Height-12, m_TextColor, DT_WORDBREAK);
}

/// Returns the ID number of the text box.
int TextBox::GetChoiceNumber() {
	return m_choiceNumber;
}

/// Returns the height (width is fixed).
long TextBox::GetHeight() {
	return m_Height;
}
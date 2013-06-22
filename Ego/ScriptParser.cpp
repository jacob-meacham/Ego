#include <windows.h>
#include <mmsystem.h>
#include <sstream>
#include "ScriptParser.h"
#include "GameArea.h"
#include "Inventory.h"
#include "Ego.h"
//////////////////////////////////////////////////////////////////////////////////
/// Standard constructor.  Resets all timer variables.
Parser::Parser() { 
	dwLastTick = 0;
    dCurTime = 0;
	dLastTime = 0;
	fConversationTimer = 0;
	fElpasedTime = 0;
	executionTime = 0;
	currentObject = NULL;
}
//////////////////////////////////////////////////////////////////////////////////
/// Constructor which sets the parent of the Parser.
Parser::Parser(GameArea *p) { pParent = p; }
//////////////////////////////////////////////////////////////////////////////////
/// Sets the parent of the Parser to p.
void Parser::SetParent(GameArea *p) { pParent = p; }
//////////////////////////////////////////////////////////////////////////////////
/// Creates an AST of the fileName (must be a .sc file).
void Parser::ParseFile(string fileName) {
	// create a file iterator
	iterator_t first(fileName.c_str());
	if(!first) {
		// if the file cannot be opened, use the generic script instead.
		int index = fileName.find('_', 0);
		// keep the action, but change the object name to generic.
		fileName.replace(0, index, "generic");
		
		iterator_t temp(fileName.c_str());
		first = temp;
	}

	// fill the AST, using AdventureScript as the grammar, and spaces as the skip parser
	// (see boost::spirit for more information on spirit).
	info = ast_parse(first, first.make_end(), adventureScript, space_p);
	if(!info.full) { 
		int index = fileName.find('_', 0);
		// keep the action, but change the object name to generic.
		fileName.replace(0, index, "generic");
		iterator_t temp(fileName.c_str());
		first = temp;

		// fill the AST, using AdventureScript as the grammar, and spaces as the skip parser
		// (see boost::spirit for more information on spirit).
		info = ast_parse(first, first.make_end(), adventureScript, space_p);

	}
	
	// Set the global iterator to the beginning of the AST.
	iScript = info.trees.begin()->children.begin();
	
	SetWaiting(false);
	executionTime = 0.0f;
}
//////////////////////////////////////////////////////////////////////////////////
// Conversation String 
// Only conversation strings begin with identifiers.
void Parser::handleIdentifier() {
	// Find the object which this identifier points to.
	Object * object = pParent->FindObject(string(iScript->value.begin(), iScript->value.end()));
	if(!object) {
		std::stringstream s;
		s << "Couldn't find " << string(iScript->value.begin(), iScript->value.end());

		TRACE((char*)s.str().c_str());
		executionTime = 0.0f;
		iScript++;
		return;
	}

	// calculate the x and y coordinates at which to print the conversation string.
	long x = object->GetXPos() - 20;
	long y = object->GetYPos() - 20;
	if(x < 0) { x = 10; }
	if(y < 0) { y = 10; }

	// create the string.
	string conv(iScript->children.begin()->value.begin(), iScript->children.begin()->value.end());

	// set the parent's current conversation string
	pParent->SetConversationString(conv, object->GetTextColor(), x, y);
	object->DoTalkingAnimation();
	currentObject = object;

	// executionTime is dependendent on the length of the string.
	executionTime = ((float)(conv.length()) * 0.05f) + .5f;
	iScript++;
}
//////////////////////////////////////////////////////////////////////////////////
void Parser::handleDoAnimation() {
	// first, find the object.
	Object * object = pParent->FindObject(string(iScript->children.begin()->value.begin(), iScript->children.begin()->value.end()));
	if(!object) {
		TRACE("Couldn't find %s", string(iScript->children.begin()->value.begin(), iScript->children.begin()->value.end()).c_str());
		executionTime = 0.0f;
		iScript++;
		return;
	}
		
	// next, find the number of the animation
	string animationNumberString((iScript->children.begin()+1)->value.begin(), (iScript->children.begin()+1)->value.end());
		
	// then, the number of seconds the parser should wait before executing the next line.
	string numSecondsToWait((iScript->children.begin()+2)->value.begin(), (iScript->children.begin()+2)->value.end());
		
	int animationNumber = atoi(animationNumberString.c_str());
	object->SetAnimation(animationNumber);
	object->SetAutoAnimate(true);	
		
	// set execution time to the appropriate time.
	executionTime = (float)(atoi(numSecondsToWait.c_str()));
	iScript++;
}
//////////////////////////////////////////////////////////////////////////////////
void Parser::handleGetItem() {
	// find the object in question
	string objectName(iScript->value.begin(), iScript->value.end());
	std::stringstream inventoryObject;
	inventoryObject << objectName << "Inventory";

	Object * object = pParent->FindObject(inventoryObject.str());
	if(!object) {
		object = pParent->FindObject(objectName);
	}
		
	if(!object) {
		TRACE("Couldn't find %s", objectName.c_str());
		executionTime = 0.0f;
		iScript++;
		return;
	}

	if(iScript->children.size() == 0) {
		// add that object to the main inventory.
		pParent->GetInventory()->AddInventoryObject(*object);

		// no time is needed to execute this line.
		executionTime = 0.0f;
	} else {		
		// add that object to the main inventory.
		pParent->GetInventory()->AddInventoryObject(*object);

		// Then, find out what the animationString says.
		string animationString((iScript->children.begin()+1)->value.begin(), (iScript->children.begin()+1)->value.end());
		int animationNumber = atoi(animationString.c_str());

		// if the number is 0, then do no animation.
		if(animationNumber = 0) {
			executionTime = 0.0f;
		}
		// if the number is one, do default pickup animation.
		else if(animationNumber = 1) {
			pParent->GetEgo()->DoPickupAnimation();
			executionTime = 0.8f;
		}

		// else, do the animation pointed to by animationString.
		else {
			pParent->GetEgo()->SetAnimation(animationNumber);
			pParent->GetEgo()->SetAutoAnimate(true);
			executionTime = pParent->GetEgo()->GetNumAnimationFrames(animationNumber) * 0.5f;
		}
	}

	// increment the iterator.
	iScript++;
}
//////////////////////////////////////////////////////////////////////////////////
void Parser::handleLoseItem() {
	// simply remove the object in question.
	pParent->GetInventory()->LoseObject(string(iScript->value.begin(), iScript->value.end()));
		
	// no time is needed to execute this line.
	executionTime = 0.0f;
	iScript++;
}
//////////////////////////////////////////////////////////////////////////////////
void Parser::handleWait() {
	string wait(iScript->value.begin(), iScript->value.end());
		
	// wait the specified number of seconds.
	executionTime = (float)atoi(wait.c_str());
	iScript++;
}
//////////////////////////////////////////////////////////////////////////////////
void Parser::handleKillObject() {
	// find the object in question, and set its visibility status to false.
	Object * object = pParent->FindObject(string(iScript->value.begin(), iScript->value.end()));
	if(!object) {
		TRACE("Couldn't find %s", string(iScript->children.begin()->value.begin(), iScript->children.begin()->value.end()).c_str());
		executionTime = 0.0f;
		iScript++;
		return;
	}

	object->SetVisible(false);

	executionTime = 0.0f;
	iScript++;
}
//////////////////////////////////////////////////////////////////////////////////
void Parser::handleCreateObject() {
	// find the object in question and set its visibilty status to true.
	if(iScript->children.size() == 0) {
		Object* object = pParent->FindObject(string(iScript->value.begin(), iScript->value.end()));
		if(!object) {
			TRACE("Couldn't find %s", string(iScript->value.begin(), iScript->value.end()).c_str());
			executionTime = 0.0f;
			iScript++;
			return;
		}

		object->SetVisible(true);
	} else {
		Object * object = pParent->FindObject(string(iScript->children.begin()->value.begin(), iScript->children.begin()->value.end()));
		if(!object) {
			TRACE("Couldn't find %s", string(iScript->children.begin()->value.begin(), iScript->children.begin()->value.end()).c_str());
			
			executionTime = 0.0f;
			iScript++;
			return;
		}

		object->SetVisible(true);
		string x((iScript->children.begin() + 1)->value.begin(), (iScript->children.begin() + 1)->value.end());
		string y((iScript->children.begin() + 2)->value.begin(), (iScript->children.begin() + 2)->value.end());
		object->SetXYPos(atoi(x.c_str()), atoi(y.c_str()));
	}
	
	executionTime = 0.0f;
	iScript++;
}
//////////////////////////////////////////////////////////////////////////////////
void Parser::handleSetVariable() {
	
	string variableType(iScript->children.begin()->value.begin(), iScript->children.begin()->value.end());
	string variableNum((iScript->children.begin()+1)->value.begin(), (iScript->children.begin()+1)->value.end());
	string setTo((iScript->children.begin()+2)->value.begin(), (iScript->children.begin()+2)->value.end());

	// if the variable to be set is a room variable, simply set the parent's flag.
	if(variableType.compare("ROOM") == 0) {
		pParent->SetFlag(atoi(variableNum.c_str()), atoi(setTo.c_str()));
	} else {
		// otherwise, find the object in question, and set the appropriate flag.
		Object * object = pParent->FindObject(variableType);
		if(!object) {
			TRACE("Couldn't find %s", variableType.c_str());
			
			executionTime = 0.0f;
			iScript++;
			return;
		}
		
		object->SetFlag(atoi(variableNum.c_str()), atoi(setTo.c_str()));
	}
	
	executionTime = 0.0f;
	iScript++;
}
//////////////////////////////////////////////////////////////////////////////////
void Parser::handleShowHideBlock() {
	while(iScript->value.id() == AdventureScript::ShowHideID) {
		// find the show/hide string
		string visible(iScript->children.begin()->value.begin(), iScript->children.begin()->value.end());
		if(visible.compare("show") == 0) {
			// find the number to show.
			string choice((iScript->children.begin()+1)->value.begin(), (iScript->children.begin()+1)->value.end());
			// show the choice.
			pParent->ShowConversationChoice(atoi(choice.c_str()));
		}
		else {
			// find the number to hide.
			string choice((iScript->children.begin()+1)->value.begin(), (iScript->children.begin()+1)->value.end());
			// hide the choice.
			pParent->HideConversationChoice(atoi(choice.c_str()));
				
		}
		// increment the iterator.
		iScript++;
	}
}
//////////////////////////////////////////////////////////////////////////////////
void Parser::handleConversation() {
	// Start of Conversation
	if(iScript->value.id() == AdventureScript::BeginConversationID) {
		// save the position of the iterator, and fill the global conversation list.
		iter_t temp = iScript;
		FillConversationList();

		// reset the position of the iterator, and execute the show/hide statements (see AdventureScript for grammar).
		iScript = temp;
		iScript++;

		handleShowHideBlock();
		
		// fill the parent's conversation choices.
		pParent->FillConversationChoices();
		SetWaiting(true);
	} else if(iScript->value.id() == AdventureScript::ShowHideID) {
		handleShowHideBlock();

		// refill the parent's conversation choices.
		pParent->FillConversationChoices();
		SetWaiting(true);
	}
	else if(iScript->value.id() == AdventureScript::ConversationHeaderID) {
		// Sets the global script to the beginning of the conversation string.
		iScript = iScript->children.begin()+1;
		iScript = iScript->children.begin();
	}
}
//////////////////////////////////////////////////////////////////////////////////
void Parser::handleIfElse() {
	if(iScript->value.id() == AdventureScript::IfBlockID) {
		// find the condition.
		string conditionString(iScript->children.begin()->value.begin(), iScript->children.begin()->value.end());

		// determine if the condition is false.
		if(pParent->GetCurActionObject()->GetFlag(atoi(conditionString.c_str())) == 0) {
			// if it is, we can iterate past the if block.
			iScript++;
			executionTime = 0.0f;
		} else {
			// otherwise, save the position of the global iterator
			iIfBlock = iScript;

			// and set the iterator to the beginning of the if block.
			iScript = (iScript->children.begin()+2);
			executionTime = 0.0f;
		}
	} else if (iScript->value.id() == AdventureScript::IfElseBlockID) {
		iIfBlock = iScript;
		
		string conditionString(iScript->children.begin()->children.begin()->value.begin(), iScript->children.begin()->children.begin()->value.end());
		if(pParent->GetCurActionObject()->GetFlag(atoi(conditionString.c_str())) == 0) {
			// set iScript to its else block child.
			iScript = (iScript->children.begin()+1);

			// set iScript to else block's first statement child.
			iScript = (iScript->children.begin()+1);
			
		} else {
			// set iScript to its if block child.
			iScript = iScript->children.begin();

			// set iScript to if block's first statement child.
			iScript = (iScript->children.begin()+2);
		}

		executionTime = 0.0f;
	} else { // Block end
		// reset the global iterator to its position before the if block.
		iScript = iIfBlock;
		
		// iterate past the if block.
		iScript++;
		executionTime = 0.0f;
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Executes the next line of the script, regardless of execution time.
/** ExecuteNextLine() is the most important function of Parser.  It should be called by
	its parent GameArea only when FinishedLine() returns true.  ExecuteNextLine() makes
	extensive use of GameArea::FindObject() to set conversation strings, animations, and flags.
	For more information on how the AST is created, see AdventureScript.
*/
void Parser::ExecuteNextLine() {
	// reset execution time
	executionTime = 0.0f;
	pParent->SetConversationString("", 0x00000000, 0, 0);

	if(iScript->value.id() == AdventureScript::IdentifierID) {
		handleIdentifier();
		return;
	} else if(iScript->value.id() == AdventureScript::DoAnimationID) {
		handleDoAnimation();
	} else if(iScript->value.id() == AdventureScript::GetItemID) {
		handleGetItem();
	} else if(iScript->value.id() == AdventureScript::LoseItemID) { 
		handleLoseItem();
	} else if(iScript->value.id() == AdventureScript::WaitID) {
		handleWait();
	} else if(iScript->value.id() == AdventureScript::KillObjectID) {
		handleKillObject();
	} else if(iScript->value.id() == AdventureScript::CreateObjectID) { 
		handleCreateObject();
	} else if(iScript->value.id() == AdventureScript::SetVariableID) { 
		handleSetVariable();
	} else if(iScript->value.id() == AdventureScript::BeginConversationID ||
			  iScript->value.id() == AdventureScript::ConversationHeaderID ||
			  iScript->value.id() == AdventureScript::ShowHideID) {
		handleConversation();
	} else if(iScript->value.id() == AdventureScript::IfBlockID ||
			iScript->value.id() == AdventureScript::IfElseBlockID ||
			string(iScript->value.begin(), iScript->value.end()).compare("}") == 0) {
		handleIfElse();
	} else if(string(iScript->value.begin(), iScript->value.end()).compare("stop") == 0) { 
		// Stop Command -- if we ever see stop, the script is at an end.
		// clear the current object of attention, and set the parent's script status to false.
		pParent->ClearCurActionObject();
		pParent->SetInScript(false);
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Determines if the Parser has finished parsing a line of script.
/** \return true if the execution time has been reached.
*/
bool Parser::FinishedLine() {
	// set timer variables.
	dCurTime = timeGetTime();
	fElpasedTime = (float)((dCurTime - dLastTime) * 0.001);
	dLastTime    = dCurTime;
	fConversationTimer += fElpasedTime;

	// if we are above the execution time, return true.
	if( fConversationTimer >= executionTime ) {
			fConversationTimer = 0.0f;
			if(currentObject != NULL) {
				currentObject->SetAnimation(currentObject->GetDefaultAnimation());
				currentObject = NULL;
			}
			return true;
	}

	// else, return false.
	return false;
}
//////////////////////////////////////////////////////////////////////////////////
/// Fills the parent GameArea's global conversation list.
/** This function parses the entire conversation contained in the script and adds each choice to
	the GameArea's list.
*/
void Parser::FillConversationList() {
	// first, there will be some show/hide statements.  we must iterate past them.
	while(iScript->value.id() != AdventureScript::ConversationHeaderID) { iScript++; }
	
	// Set an iterator to the beginning of the conversation.
	iConversation = iScript;
	
	// while still in conversation...
	while(iScript->value.id() != AdventureScript::EndConversationID) {
		// determine choice number.
		string choiceNumber(iScript->value.begin(), iScript->value.end());

		// determine choice string.
		string choiceString(iScript->children.begin()->value.begin(), iScript->children.begin()->value.end());

		// Add the choice, default visibilty is not visible.
		pParent->AddConversationChoice(atoi(choiceNumber.c_str()), choiceString, false);

		// increment the iterator.
		iScript++;
	} // end while.
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the waiting status of the iterator.
bool Parser::WaitingForInput() const { return waiting; }
//////////////////////////////////////////////////////////////////////////////////
/// Sets the waiting status of the iterator to d.
void Parser::SetWaiting(bool d) { waiting = d; }
//////////////////////////////////////////////////////////////////////////////////
/// Jumps the iterator to a conversation choice.
void Parser::JumpToConversation(int number) {
	// set the global iterator to the beginning of the conversation block.
	iScript = iConversation;

	// Determine the choice number of the script choice
	string choiceNumber(iScript->value.begin(), iScript->value.end());

	// compare it with the number to jump to.
	// while this comparison is not true...
	while(atoi(choiceNumber.c_str()) != number) {
		// increment the iterator.
		iScript++;
		choiceNumber.clear();
		// Determine the new choice number.
		choiceNumber = string(iScript->value.begin(), iScript->value.end());
	}
	// Set waiting to false, since there may be some commands to execute.
	SetWaiting(false);
}
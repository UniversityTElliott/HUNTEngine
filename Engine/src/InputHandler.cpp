

#include <HUNT/Input/InputHandler.h>
#include <SDL3/SDL_keyboard.h>

namespace HUNT
{
	//maybe use smaller data types int is kinda big for this?

	void InputHandler::update() 
	{

		//set new previous to old current (kinda confusing)
		previous = current;

		int count = 0;
		//Raw pointer cuz SDL own this not me
		const bool* state = SDL_GetKeyboardState(&count);
		//Copy the internal SDL state
		for (int i = 0; i < current.size(); ++i)
		{

			current[i] = (i < count) && state[i];
		}
	}

	bool InputHandler::isHeld(HUNT::Key key) const
	{

		//Only return if the key is in not invalid ID and in the array
		return (key < current.size()) && (current[key]);
	}

	bool InputHandler::wasPressed(HUNT::Key key) const
	{
		//Only return if the key is not invalid ID and its ONLY in current and not previous
		return (key < current.size()) && (current[key]) && (!previous[key]);
	}

	bool InputHandler::wasReleased(HUNT::Key key) const
	{
		//Opposite of above
		return (key < current.size()) && (!current[key]) && (previous[key]);
	}
}
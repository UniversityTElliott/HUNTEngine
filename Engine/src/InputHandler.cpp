

#include <HUNT/Input/InputHandler.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <memory>


namespace HUNT
{
	//maybe use smaller data types int is kinda big for this?
	struct InputHandler::Impl {
		

		//arrays of every current and previous key and if it was pressed
		std::array<bool, SDL_SCANCODE_COUNT> current{};
		std::array<bool, SDL_SCANCODE_COUNT> previous{};

		//Mouse works imilarly to keys but it returns a bitmask so use this format
		SDL_MouseButtonFlags currentMouse{};
		SDL_MouseButtonFlags previousMouse{};

		//SDL takes a reference and fills it? why bro
		float mouseX = 0;
		float mouseXDifference = 0;
		float mouseY = 0;
		float mouseYDifference = 0;

	};


	InputHandler::InputHandler() : impl_(std::make_unique<Impl>()) {}

	/*
	Renderer::Renderer(SDL_Window* window)
        : impl_(std::make_unique<Impl>(window))
    {*/

	InputHandler::~InputHandler() = default;

	float InputHandler::getMouseX()
	{
		return impl_->mouseX;
	}

	float InputHandler::getMouseY()
	{
		return impl_->mouseY;
	}

	float InputHandler::getMouseXDifference()
	{
		return impl_->mouseXDifference;
	}
	float InputHandler::getMouseYDifference()
	{
		return impl_->mouseYDifference;
	}

	void InputHandler::update()
	{
		//----------KEYS------------
		//set new previous to old current (kinda confusing)
		impl_->previous = impl_->current;

		int count = 0;
		//Raw pointer cuz SDL own this not me
		const bool* state = SDL_GetKeyboardState(&count);
		//Copy the internal SDL state
		for (int i = 0; i < impl_->current.size(); ++i)
		{

			impl_->current[i] = (i < count) && state[i];
		}

		//-----------------MOUSE--------------

		impl_->previousMouse = impl_->currentMouse;

		// Get current mouse buttons and position
		impl_->currentMouse = SDL_GetMouseState(&impl_->mouseX, &impl_->mouseY);

		//Store the change in X and Y
		float xIn, yIn;
		SDL_GetRelativeMouseState(&xIn, &yIn);

		impl_->mouseXDifference = xIn;
		impl_->mouseYDifference = yIn;


	}

	bool InputHandler::isHeld(HUNT::Input inputIn) const
	{

		//Only return if the key is in not invalid ID and in the array
		switch (inputIn.type) {
		case InputType::Keyboard:
			return (inputIn.idCode < impl_->current.size()) && (impl_->current[inputIn.idCode]);
		
		case InputType::Mouse:
			return (impl_->currentMouse & inputIn.idCode) != 0;
		}


		return false;
	}

	bool InputHandler::wasPressed(HUNT::Input inputIn) const
	{
		//Only return if the key is not invalid ID and its ONLY in current and not previous
		switch (inputIn.type) {
		case InputType::Keyboard:
			return (inputIn.idCode < impl_->current.size()) && (impl_->current[inputIn.idCode]) && (!impl_->previous[inputIn.idCode]);

		case InputType::Mouse:
			return (impl_->currentMouse & inputIn.idCode) != 0 &&(impl_->previousMouse & inputIn.idCode) == 0;
		}

		//inputIn.idCode
		return false;
	}

	bool InputHandler::wasReleased(HUNT::Input inputIn) const
	{

		switch (inputIn.type) {
		case InputType::Keyboard:
			return (inputIn.idCode < impl_->current.size()) && (!impl_->current[inputIn.idCode]) && (impl_->previous[inputIn.idCode]);

		case InputType::Mouse:
			return (impl_->currentMouse & inputIn.idCode) == 0 && (impl_->previousMouse & inputIn.idCode) != 0;
		}

		//Opposite of above
		return false;
	}
}
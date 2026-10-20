

#include <HUNT/Input/InputHandler.h>
#include <SDL3/SDL_keyboard.h>
#include <memory>

namespace HUNT
{
	//maybe use smaller data types int is kinda big for this?
	struct InputHandler::Impl {
		

		//arrays of every current and previous key and if it was pressed
		std::array<bool, SDL_SCANCODE_COUNT> current{};
		std::array<bool, SDL_SCANCODE_COUNT> previous{};

	};


	InputHandler::InputHandler() : impl_(std::make_unique<Impl>()) {}

	/*
	Renderer::Renderer(SDL_Window* window)
        : impl_(std::make_unique<Impl>(window))
    {*/

	InputHandler::~InputHandler() = default;
	

	void InputHandler::update()
	{

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
	}

	bool InputHandler::isHeld(HUNT::Key key) const
	{

		//Only return if the key is in not invalid ID and in the array
		return (key < impl_->current.size()) && (impl_->current[key]);
	}

	bool InputHandler::wasPressed(HUNT::Key key) const
	{
		//Only return if the key is not invalid ID and its ONLY in current and not previous
		return (key < impl_->current.size()) && (impl_->current[key]) && (!impl_->previous[key]);
	}

	bool InputHandler::wasReleased(HUNT::Key key) const
	{
		//Opposite of above
		return (key < impl_->current.size()) && (!impl_->current[key]) && (impl_->previous[key]);
	}
}
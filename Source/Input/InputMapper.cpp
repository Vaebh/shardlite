#include "InputMapper.h"
#include "InputConstants.h"
#include "MappedInput.h"

#include "../Systems/SetupUtils.h"

#include "../Utils/MathUtils.h"

#include <iostream>

#include <SDL.h>

namespace
{
	void AddRawMouseInput(std::vector<MappedInput>& mappedInput)
	{
		MappedInput mouseInput;
		mouseInput.m_rawInput = INPUT_MOUSE_MOTION;

		int x;
		int y;
		SDL_GetRelativeMouseState(&x, &y);

		mouseInput.m_rangeInputValue.x = x;
		mouseInput.m_rangeInputValue.y = y;

		mappedInput.push_back(mouseInput);
	}

	void AddRawKeyInput(std::vector<MappedInput>& mappedInput)
	{
		const Uint8* keystate = SDL_GetKeyboardState(NULL);

		// This is our accepted range
		for (int i = INPUT_SCANCODE_A; i <= INPUT_SCANCODE_UP; ++i)
		{
			if (keystate[i])
			{
				MappedInput keyInput;
				keyInput.m_rawInput = (RawInput)i;
				mappedInput.push_back(keyInput);
			}
		}
	}

	void AddMouseClickInput(SDL_Event sdlEvent, std::vector<MappedInput>& mappedInput)
	{
		MappedInput mouseClickInput;
		if (sdlEvent.button.button == SDL_BUTTON_LEFT)
		{
			mouseClickInput.m_rawInput = INPUT_LEFT_CLICK_PRESS;
		}
		else if (sdlEvent.button.button == SDL_BUTTON_RIGHT)
		{
			mouseClickInput.m_rawInput = INPUT_RIGHT_CLICK_PRESS;
		}
		else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE)
		{
			mouseClickInput.m_rawInput = INPUT_MIDDLE_CLICK_PRESS;
		}

		mappedInput.push_back(mouseClickInput);
	}
}

int InputMapper::StartUp()
{
	SDL_SetRelativeMouseMode(SDL_TRUE);

	SetupGameControllers();

	return SETUP_SUCCESS;
}

int InputMapper::ShutDown()
{
	return SETUP_SUCCESS;
}

void InputMapper::SetupGameControllers()
{
	int MaxJoysticks = SDL_NumJoysticks();
	int ControllerIndex = 0;

	for (int JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
	{
		if (!SDL_IsGameController(JoystickIndex))
		{
			continue;
		}
		if (ControllerIndex >= MAX_CONTROLLERS)
		{
			break;
		}

		m_controllerHandles[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);

		// Detection code for xinput, xinput is in the name itself. Use when actually
		// doing the direct and x input mapping
		/*SDL_Joystick* m_joy = SDL_JoystickOpen(JoystickIndex);
		std::string name = SDL_JoystickName(m_joy);
		std::cout << name.c_str() << std::endl;*/

		ControllerIndex++;
	}

	m_numControllers = ControllerIndex;
}

void InputMapper::UpdateInput()
{
	std::vector<MappedInput> mappedInput;

	GetRawInput(mappedInput);
	MapRawInput(mappedInput);
	ProcessMappedInput(mappedInput);
}

void InputMapper::GetRawInput(std::vector<MappedInput>& mappedInput)
{
	AddRawMouseInput(mappedInput);
	AddRawKeyInput(mappedInput);
	AddControllerTriggerInput(mappedInput);

	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				AddMouseClickInput(sdlEvent, mappedInput);
				break;

			case SDL_CONTROLLERBUTTONDOWN:
				MappedInput controllerInput;
				controllerInput.m_rawInput = (RawInput)(sdlEvent.cbutton.button + (int)INPUT_CONTROLLER_BUTTON_A);
				mappedInput.push_back(controllerInput);
				break;
		}
	}
}

void InputMapper::MapRawInput(std::vector<MappedInput>& mappedInput)
{
	// A double loop is unfortunate
	for (int i = 0; i < mappedInput.size(); ++i)
	{
		for (int j = 0; j < m_inputContexts.size(); ++j)
		{
			if (m_inputContexts[j].MapInput(mappedInput[i]))
			{
				break;
			}
		}
	}
}

void InputMapper::ProcessMappedInput(std::vector<MappedInput>& mappedInput)
{
	// double loop again, urg
	for (int i = 0; i < mappedInput.size(); ++i)
	{
		for (int j = 0; j < m_inputCallbacks.size(); ++j)
		{
			(*m_inputCallbacks[j])(mappedInput[i]);
		}
	}
}

void InputMapper::AddContext(InputContext& in_context)
{
	m_inputContexts.push_back(in_context);
}

void InputMapper::SubscribeToInput(IInputCallback* callback)
{
	m_inputCallbacks.push_back(callback);
}

// Will have to eventually handle DirectInput only having one shared axis, using XInput separate axes for right now
void InputMapper::AddControllerTriggerInput(std::vector<MappedInput>& mappedInput)
{
	Sint16 axis = 0;
	float normalizedAxis = 0.f;

	for (int i = 0; i < m_numControllers; ++i)
	{
		// left trigger
		{
			MappedInput leftTrigger;
			leftTrigger.m_rawInput = INPUT_CONTROLLER_LEFT_TRIGGER;

			axis = SDL_GameControllerGetAxis(m_controllerHandles[i], SDL_CONTROLLER_AXIS_TRIGGERLEFT);
			leftTrigger.m_inputValue = LinearMap(axis, 0, 32767, 0, 1);

			mappedInput.push_back(leftTrigger);
		}

		// right trigger
		{
			MappedInput rightTrigger;
			rightTrigger.m_rawInput = INPUT_CONTROLLER_RIGHT_TRIGGER;

			axis = SDL_GameControllerGetAxis(m_controllerHandles[i], SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
			rightTrigger.m_inputValue = LinearMap(axis, 0, 32767, 0, 1);

			mappedInput.push_back(rightTrigger);
		}
	}
}
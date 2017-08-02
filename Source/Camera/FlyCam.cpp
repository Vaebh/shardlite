#include "FlyCam.h"

#include <SDL.h>

#include <iostream>

FlyCamera::FlyCamera() :
Camera(),
m_moveSpeed(0.005f),
m_rotationSpeed(100.f)
{
	m_rotationConstraints = glm::vec3(0.97f, 0.f, 0.f);
    
#ifdef __APPLE__
    m_moveSpeed *= 0.1f;
    m_rotationSpeed *= 0.1f;
#endif
}

void FlyCamera::Init(SDL_Window* window)
{
	m_window = window;
}

void FlyCamera::Update(float deltaTime)
{
	/*const Uint8* keystate = SDL_GetKeyboardState(NULL);

	if (keystate[SDL_SCANCODE_W])
	{
		//m_position += m_direction * (m_moveSpeed * deltaTime) * (m_inverted ? -1.f : 1.f);
	}
	if (keystate[SDL_SCANCODE_S])
	{
		m_position -= m_direction * (m_moveSpeed * deltaTime) * (m_inverted ? -1.f : 1.f);
	}
	if (keystate[SDL_SCANCODE_A])
	{
		m_position += m_right * (m_moveSpeed * deltaTime) * (m_inverted ? 1.f : -1.f);
	}
	if (keystate[SDL_SCANCODE_D])
	{
		m_position -= m_right * (m_moveSpeed * deltaTime) * (m_inverted ? 1.f : -1.f);
	}

	if (keystate[SDL_SCANCODE_SPACE])
	{
		m_position -= m_up * ((m_inverted ? 1.f : -1.f) * m_moveSpeed * deltaTime);
	}*/

	bool mouseGrabbed = false;
	SDL_Event event;
	/*while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_MOUSEMOTION:
			int x;
			int y;
			if (!mouseGrabbed)
			{
				SDL_GetRelativeMouseState(&x, &y);

				RotatePitch(glm::radians((float)(y) * m_rotationSpeed));
				RotateYaw(glm::radians((float)(x) * m_rotationSpeed));

				SDL_WarpMouseInWindow(m_window, (short)(640 / 2), ((short)480 / 2));
			}
			else
			{
				// Soak up the mouse event generated by warping the mouse back to the middle
				SDL_GetRelativeMouseState(&x, &y);
			}

			mouseGrabbed = !mouseGrabbed;
			break;
		}
	}*/

	//std::cout << "Direction: " << m_direction.x << ", " << m_direction.y << ", " << m_direction.z << std::endl;
	//std::cout << "Right: " << m_right.x << ", " << m_right.y << ", " << m_right.z << std::endl;
	//std::cout << "Up: " << m_up.x << ", " << m_up.y << ", " << m_up.z << std::endl << std::endl;
}

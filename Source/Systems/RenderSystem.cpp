#include "RenderSystem.h"

#include "../Rendering/GLIncludes.h"

#include "../Rendering/System/Batch.h"
#include "../Rendering/MeshManagement/MeshComponentManager.h"

#include <SDL.h>

#include <iostream>

void RenderSystem::SetManagerReferences(MeshComponentManager* meshComponentManager, SDL_Window* gameWindow)
{
	m_meshComponentManager = meshComponentManager;
	m_gameWindow = gameWindow;
}

int RenderSystem::StartUp()
{
#ifdef _WIN32
	glewExperimental = GL_TRUE;

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cout << "Glew failed init" << std::endl;
		std::cout << "Error initializing GLEW! %s\n" << glewGetErrorString(glewError) << std::endl;
		std::cin.get();
		return 1;
	}
#endif

	return SETUP_SUCCESS;
}

int RenderSystem::ShutDown()
{
	return SETUP_SUCCESS;
}

void RenderSystem::Draw()
{
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	std::vector<Batch> batches = m_meshComponentManager->GetOpaqueBatches();
	Batch testBatch = batches[0];
	testBatch.GenerateBatchData();

	glDrawArrays(GL_TRIANGLES, 0, testBatch.GetVertexCount());

	SDL_GL_SwapWindow(m_gameWindow);

#ifdef _WIN32
	GLuint errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
		printf("%i, %s\n", errorCode, gluErrorString(errorCode));
#endif
}
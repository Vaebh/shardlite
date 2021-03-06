#ifndef SCENE_SHARDLITE
#define SCENE_SHARDLITE

//#include <SDL.h>

#include "../Systems/SDLHandler.h"

class MeshComponentManager;
class ShaderCache;
class TextureManager;
class InputMapper;
class HapticsSystem;

class Scene
{
public:
	Scene();
	virtual ~Scene() {}

	Scene(const Scene&) =delete;
	Scene& operator=(const Scene&) =delete;

	void SetManagerReferences(MeshComponentManager* meshComponentManager,
		ShaderCache* shaderCache,
		TextureManager* textureManager,
		SDL_Window* gameWindow,
		InputMapper* inputMapper,
		HapticsSystem* hapticsSystem);

	virtual void SetupScene() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void ShutdownScene() = 0;

public:
	MeshComponentManager* m_meshCompManager;
	ShaderCache* m_shaderCache;
	TextureManager* m_textureManager;

	SDL_Window* m_gameWindow;

	InputMapper* m_inputMapper;
	HapticsSystem* m_hapticsSystem;
};

#endif
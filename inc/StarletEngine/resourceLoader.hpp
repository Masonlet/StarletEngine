#pragma once

#include "StarletGraphics/renderer.hpp"
#include "StarletScene/sceneManager.hpp"

struct Model;
struct TextureData;

class ResourceLoader {
public:
	ResourceLoader(Renderer& rendererIn) : renderer(rendererIn) {};

	bool loadMeshes(const std::vector<Model*>& models);
	bool loadTextures(const std::vector<TextureData*>& textures);

	bool processTextureConnections(SceneManager& sceneManager);
	bool processPrimitives(SceneManager& sceneManager);
	bool processGrids(SceneManager& sceneManager);

private:
	Renderer& renderer;
};
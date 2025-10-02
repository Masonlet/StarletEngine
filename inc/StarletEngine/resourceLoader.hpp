#pragma once

#include "StarletGraphics/manager/shaderManager.hpp"
#include "StarletGraphics/manager/meshManager.hpp"
#include "StarletGraphics/manager/textureManager.hpp"

struct Primitive;
struct Model;
struct TextureData;
struct Grid;

struct TransformComponent;
struct ColourComponent;

class SceneManager;

class ResourceLoader {
public:
	ResourceLoader(MeshManager& mm, TextureManager& tm) : meshManager{ mm }, textureManager{ tm } {};

	bool loadMeshes(const std::vector<Model*>& models);
	bool loadTextures(const std::vector<TextureData*>& textures);

	bool processPrimitives(SceneManager& sm);
	bool processGrids(SceneManager& sm);

	bool createPrimitiveMesh(const Primitive& primitive, const TransformComponent& transform, const ColourComponent& colour);
	bool createGridMesh(const Grid& grid, const std::string& meshName, const TransformComponent& transform, const ColourComponent& colour);

private:
	MeshManager& meshManager;
	TextureManager& textureManager;
};
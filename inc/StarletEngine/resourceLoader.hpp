#pragma once

#include "StarletGraphics/shader/shaderManager.hpp"
#include "StarletGraphics/mesh/meshManager.hpp"
#include "StarletGraphics/texture/textureManager.hpp"

struct Model;
struct TextureData;
struct Primitive;
struct Grid;
struct TransformComponent;
class SceneManager;

class ResourceLoader {
public:
	ResourceLoader(ShaderManager& sm, MeshManager& mm, TextureManager& tm) : shaderManager{ sm }, meshManager{ mm }, textureManager{ tm } {};

	bool loadMeshes(const std::vector<Model*>& models);
	bool loadTextures(const std::vector<TextureData*>& textures);

	bool processTextureConnections(SceneManager& sm);
	bool processPrimitives(SceneManager& sm);
	bool processGrids(SceneManager& sm);

	bool createPrimitiveMesh(const Primitive& primitive, const TransformComponent& transform);
	bool createGridMesh(const Grid& grid, const TransformComponent& transform, const std::string& meshName);

private:
	ShaderManager& shaderManager;
	MeshManager& meshManager;
	TextureManager& textureManager;
};
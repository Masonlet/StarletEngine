#include "StarletEngine/resourceLoader.hpp"

#include "StarletScene/sceneManager.hpp"
#include "StarletScene/components/transform.hpp"
#include "StarletScene/components/model.hpp"
#include "StarletScene/components/light.hpp"
#include "StarletScene/components/camera.hpp"
#include "StarletScene/components/grid.hpp"
#include "StarletScene/components/textureData.hpp"
#include "StarletScene/components/textureConnection.hpp"
#include "StarletScene/components/primitive.hpp"

#include "StarletParser/utils/log.hpp"

bool ResourceLoader::loadMeshes(const std::vector<Model*>& models) {
  for (const Model* model : models)
    if (!meshManager.loadAndAddMesh(model->meshPath))
      return error("Renderer", "addMeshes", "Failed to load/add mesh: " + model->meshPath);

  return debugLog("Renderer", "addMeshes", "Added " + std::to_string(models.size()) + " meshes");
}
bool ResourceLoader::loadTextures(const std::vector<TextureData*>& textures) {
  for (const TextureData* texture : textures) {
    if (!texture->isCube) {
      if (!textureManager.addTexture(texture->name, texture->faces[0]))
        return error("Renderer", "loadSceneTextures", "Failed to load 2D texture: " + texture->name);
    }
    else if (!textureManager.addTextureCube(texture->name, texture->faces))
      return error("Renderer", "loadSceneTextures", "Failed to load cube map: " + texture->name);
  }

  return debugLog("Renderer", "addTextures", "Added " + std::to_string(textures.size()) + " textures");
}

bool ResourceLoader::createPrimitiveMesh(const Primitive& primitive, const TransformComponent& transform, const ColourComponent& colour) {
  switch (primitive.type) {
  case PrimitiveType::Triangle:
    return meshManager.createTriangle(primitive.name, { transform.size.x, transform.size.y }, colour.colour);
  case PrimitiveType::Square:
    return meshManager.createSquare(primitive.name, { transform.size.x, transform.size.y }, colour.colour);
  case PrimitiveType::Cube:
    return meshManager.createCube(primitive.name, transform.size, colour.colour);
  default:
    return error("Renderer", "loadScenePrimitives", "Invalid primitive: " + primitive.name);
  }
}
bool ResourceLoader::createGridMesh(const Grid& grid, const std::string& meshName, const TransformComponent& transform, const ColourComponent& colour) {
  switch (grid.type) {
  case GridType::Square:
    return meshManager.createSquare(meshName, { transform.size.x, transform.size.y }, colour.colour);
  case GridType::Cube:
    return meshManager.createCube(meshName, transform.size, colour.colour);
  default:
    return error("Renderer", "createGridMesh", "Invalid grid: " + grid.name + ", mesh: " + meshName);
  }
}

bool ResourceLoader::processPrimitives(SceneManager& sm) {
  for (Primitive* primitive : sm.getScene().getComponentsOfType<Primitive>()) {
    const StarEntity entity = primitive->id;
    if (!sm.getScene().hasComponent<TransformComponent>(entity))
      return error("Engine", "loadScenePrimitives", "Primitive entity has no transform component.");

    const TransformComponent& transform = sm.getScene().getComponent<TransformComponent>(entity);

    if (sm.getScene().hasComponent<ColourComponent>(entity)) {
			const ColourComponent& colour = sm.getScene().getComponent<ColourComponent>(entity);
      if (!createPrimitiveMesh(*primitive, transform, colour))
        return error("Engine", "loadScenePrimitives", "Failed to create mesh for primitive: " + primitive->name);
    } 
    else {
      const ColourComponent colour{};
      if (!createPrimitiveMesh(*primitive, transform, colour))
        return error("Engine", "loadScenePrimitives", "Failed to create mesh for primitive: " + primitive->name);
		}
			

    MeshGPU* primMesh;
    if (!meshManager.getMeshGPU(primitive->name, primMesh))
      return error("Engine", "loadScenePrimitives", "Failed to load primitive mesh: " + primitive->name);

    Model* model = sm.getScene().addComponent<Model>(entity);
    if (!model) return error("Engine", "loadScenePrimitives", "Failed to create model component for primitive: " + primitive->name);

    model->name = primitive->name;
    model->meshPath = primitive->name;
    model->useTextures = false;
    for (unsigned i = 0; i < Model::NUM_TEXTURES; ++i) {
      model->textureNames[i].clear();
      model->textureMixRatio[i] = 0.0f;
    }
  }

  return true;
}
bool ResourceLoader::processGrids(SceneManager& sceneManager) {
  for (const Grid* grid : sceneManager.getScene().getComponentsOfType<Grid>()) {
    std::string sharedName = grid->name + (grid->type == GridType::Square ? "_sharedSquare" : "_sharedCube");

    const StarEntity entity = grid->id;
    if (!sceneManager.getScene().hasComponent<TransformComponent>(entity))
      return error("Engine", "loadSceneGrids", "Grid entity has no transform component.");

    const TransformComponent& gridTransform = sceneManager.getScene().getComponent<TransformComponent>(entity);

    if(sceneManager.getScene().hasComponent<ColourComponent>(entity)) {
      const ColourComponent& colour = sceneManager.getScene().getComponent<ColourComponent>(entity);
      if (!createGridMesh(*grid, sharedName, gridTransform, colour))
        return error("Engine", "loadSceneGrids", "Failed to create mesh for: " + sharedName);
    }
    else {
      const ColourComponent colour{};
      if (!createGridMesh(*grid, sharedName, gridTransform, colour))
        return error("Engine", "loadSceneGrids", "Failed to create mesh for: " + sharedName);
		} 

    const int gridSide = (grid->count > 0) ? static_cast<int>(std::ceil(std::sqrt(static_cast<float>(grid->count)))) : 0;
    for (int i = 0; i < 0 + grid->count; ++i) {
      const int localIdx = i - 0;
      const int row = (gridSide > 0) ? (localIdx / gridSide) : 0;
      const int col = (gridSide > 0) ? (localIdx % gridSide) : 0;

      Vec3<float> pos{};
      if (grid->type == GridType::Square) {
        pos = { grid->spacing * static_cast<float>(col),
                grid->spacing * static_cast<float>(row),
                0.0f };
      }
      else {
        pos = { grid->spacing * static_cast<float>(col),
                0.0f,
                grid->spacing * static_cast<float>(row) };
      }

      StarEntity e = sceneManager.getScene().createEntity();

      TransformComponent* transform = sceneManager.getScene().addComponent<TransformComponent>(e);
      if (!transform) return error("Engine", "loadSceneGrids", "Failed to add TransformComponent for grid instance: " + grid->name);

      Model* model = sceneManager.getScene().addComponent<Model>(e);
      if (!model) return error("Engine", "loadSceneGrids", "Failed to add grid instance model: " + sharedName);

      model->name = grid->name + "_instance_" + std::to_string(i);
      model->meshPath = sharedName;
      model->useTextures = false;
      for (unsigned ti = 0; ti < Model::NUM_TEXTURES; ++ti) {
        model->textureNames[ti].clear();
        model->textureMixRatio[ti] = 0.0f;
      }
    }
  }

  return true;
}
#include "StarletEngine/engine.hpp"

#include "StarletScene/components/transform.hpp"
#include "StarletScene/components/model.hpp"
#include "StarletScene/components/light.hpp"
#include "StarletScene/components/camera.hpp"
#include "StarletScene/components/grid.hpp"
#include "StarletScene/components/textureData.hpp"
#include "StarletScene/components/textureConnection.hpp"
#include "StarletScene/components/primitive.hpp"

#include "StarletScene/systems/cameraMoveSystem.hpp"
#include "StarletScene/systems/cameraLookSystem.hpp"
#include "StarletScene/systems/cameraFovSystem.hpp"

#include <GLFW/glfw3.h>

bool Engine::loadScene(const std::string& sceneIn) {
  debugLog("Engine", "loadScene", "Start time: " + std::to_string(glfwGetTime()));

  if (sceneIn.empty()) {
    if (!sceneManager.loadTxtScene("EmptyScene.txt"))
      return error("Engine", "loadSceneMeshes", "No scene loaded and failed to load Default \"EmptyScene\"");
  }
  else if (!sceneManager.loadTxtScene(sceneIn + ".txt"))
    return error("Engine", "setScene", "Failed to load scene: " + sceneIn);

  if (renderer.getProgram() == 0) return error("Engine", "loadScene", "No active shader program set after loading scene");

  bool ok = true;

  debugLog("Engine", "loadSceneMeshes", "Start time: " + std::to_string(glfwGetTime()));
  ok &= renderer.addMeshes(sceneManager.getScene().getComponentsOfType<Model>());
  debugLog("Engine", "loadSceneMeshes", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("Engine", "loadSceneLighting", "Start time: " + std::to_string(glfwGetTime()));
  renderer.updateLightUniforms(sceneManager.getScene());
  debugLog("Engine", "loadSceneLighting", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("Engine", "loadSceneTextures", "Start time: " + std::to_string(glfwGetTime()));
  ok &= renderer.addTextures(sceneManager.getScene().getComponentsOfType<TextureData>());
  debugLog("Engine", "loadSceneTextures", "Finish time: " + std::to_string(glfwGetTime()));

  ok &= loadSceneTextureConnections();
  ok &= loadScenePrimitives();
  ok &= loadSceneGrids();

  sceneManager.getScene().registerSystem(std::make_unique<CameraMoveSystem>());
	sceneManager.getScene().registerSystem(std::make_unique<CameraLookSystem>());
	sceneManager.getScene().registerSystem(std::make_unique<CameraFovSystem>());

  return ok
    ? debugLog("Engine", "loadScene", "Finish Time: " + std::to_string(glfwGetTime()))
    : error("Engine", "loadScene", "Failed to load scene: " + sceneIn);;
}

bool Engine::loadSceneTextureConnections() {
  debugLog("Engine", "loadSceneTextureConnections", "Start time: " + std::to_string(glfwGetTime()));

  for (const TextureConnection* connection : sceneManager.getScene().getComponentsOfType<TextureConnection>()) {
    if (connection->slot >= Model::NUM_TEXTURES) 
      return error("Engine", "loadSceneTextureConnection", "Slot out of range: " + std::to_string(connection->slot));

    Model* model { sceneManager.getScene().getComponentByName<Model>(connection->modelName) };
    if (!model) return error("Engine", "loadSceneTextureConnection", "Model " + connection->modelName + " not found for connection " + connection->name);

    if (connection->textureName.empty() || connection->mix <= 0.0f) {
      model->textureNames[connection->slot].clear();
      model->textureMixRatio[connection->slot] = 0.0f;

      bool any = false;
      for (unsigned int i = 0; i < Model::NUM_TEXTURES; ++i) {
        if (!model->textureNames[i].empty()) {
          any = true;
          break;
        }
      }

      model->useTextures = any;
      debugLog("Scene", "loadSceneTextureConnections", "unbind: " + connection->modelName + "[slot " + std::to_string(connection->slot) + "]");
      continue;
    }

    TextureData* texture { sceneManager.getScene().getComponentByName<TextureData>(connection->textureName) };
    if (!texture) return error("Engine", "loadSceneTextureConnection", "Texture " + connection->textureName + " not found for connection " + connection->name);

    model->useTextures = true;
    model->textureNames[connection->slot] = connection->textureName;
    model->textureMixRatio[connection->slot] = (connection->mix < 0.0f) ? 0.0f : (connection->mix > 1.0f ? 1.0f : connection->mix);
  }
  return debugLog("Engine", "loadSceneTextureConnections", "Finish time: " + std::to_string(glfwGetTime()));
}
bool Engine::loadScenePrimitives() {
  debugLog("Engine", "loadScenePrimitives", "Start time: " + std::to_string(glfwGetTime()));

  for (Primitive* primitive : sceneManager.getScene().getComponentsOfType<Primitive>()) {
    const StarEntity entity = primitive->id;
    if (!sceneManager.getScene().hasComponent<TransformComponent>(entity)) 
      return error("Engine", "loadScenePrimitives", "Primitive entity has no transform component.");

    const TransformComponent& transform = sceneManager.getScene().getComponent<TransformComponent>(entity);

    if (!renderer.createPrimitiveMesh(*primitive, transform))
      return error("Engine", "loadScenePrimitives", "Failed to create mesh for primitive: " + primitive->name);

    MeshGPU* primMesh;
    if (!renderer.getMesh(primitive->name, primMesh))
      return error("Engine", "loadScenePrimitives", "Failed to load primitive mesh: " + primitive->name);

    Model* model = sceneManager.getScene().addComponent<Model>(entity);
    if (!model) return error("Engine", "loadScenePrimitives", "Failed to create model component for primitive: " + primitive->name);

    model->name = primitive->name;
    model->meshPath = primitive->name;
    model->useTextures = false;
    for (unsigned i = 0; i < Model::NUM_TEXTURES; ++i) {
      model->textureNames[i].clear();
      model->textureMixRatio[i] = 0.0f;
    }
    model->colour = primitive->colour;
    model->colourMode = primitive->colourMode;
  }

  return debugLog("Engine", "loadScenePrimitives", "Finish time: " + std::to_string(glfwGetTime()));
}
bool Engine::loadSceneGrids() {
  debugLog("Engine", "loadSceneGrids", "Start time: " + std::to_string(glfwGetTime()));

  for (const Grid* grid : sceneManager.getScene().getComponentsOfType<Grid>()) {
    std::string sharedName = grid->name + (grid->type == GridType::Square ? "_sharedSquare" : "_sharedCube");

    const StarEntity entity = grid->id;
    if (!sceneManager.getScene().hasComponent<TransformComponent>(entity)) 
      return error("Engine", "loadSceneGrids", "Grid entity has no transform component.");
    
    const TransformComponent& gridTransform = sceneManager.getScene().getComponent<TransformComponent>(entity);

    if (!renderer.createGridMesh(*grid, gridTransform, sharedName))
      return error("Engine", "loadSceneGrids", "Failed to create mesh for: " + sharedName);

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
      model->colour = grid->colour;
      model->colourMode = grid->colourMode;
    }
  }

  return debugLog("Engine", "loadSceneGrids", "Finish time: " + std::to_string(glfwGetTime()));
}
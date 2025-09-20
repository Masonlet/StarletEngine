#include "StarletEngine/engine.hpp"
#include <GLFW/glfw3.h>

bool Engine::loadScene(const std::string& sceneIn) {
  debugLog("Engine", "loadScene", "Start time: " + std::to_string(glfwGetTime()), true);

  if (sceneIn.empty() && !sceneManager.loadTxtScene("EmptyScene.txt"))
    return error("Engine", "loadSceneMeshes", "No scene loaded and failed to load Default \"EmptyScene\"");
  else if (!sceneManager.loadTxtScene(sceneIn + ".txt"))
    return error("Engine", "setScene", "Failed to load scene: " + sceneIn);

  if (renderer.getProgram() == 0) return error("Engine", "loadScene", "No active shader program set after loading scene");

  bool ok = true;

  ok &= loadSceneMeshes();
  ok &= loadSceneLighting();
  ok &= loadSceneTextures();
  ok &= loadSceneTextureConnections();
  ok &= loadScenePrimitives();
  ok &= loadSceneGrids();

  return ok
    ? debugLog("Engine", "loadScene", "Finish Time: " + std::to_string(glfwGetTime()), true)
    : error("Engine", "loadScene", "Failed to load scene: " + sceneIn);;
}

bool Engine::loadSceneMeshes() {
  debugLog("Engine", "loadSceneMeshes", "Start time: " + std::to_string(glfwGetTime()), true);

  return renderer.addMeshes(sceneManager.getScene().getObjects<Model>()) 
    ? debugLog("Engine", "loadSceneMeshes", "Finish time: " + std::to_string(glfwGetTime()), true) 
    : error("Engine", "loadSceneMeshes", "Failed to load meshes");
}
bool Engine::loadSceneLighting() {
  debugLog("Engine", "loadSceneLighting", "Start time: " + std::to_string(glfwGetTime()), true);
  renderer.updateLightCount(sceneManager.getScene().getObjectCount<Light>());
  renderer.updateLightUniforms(sceneManager.getScene().getObjects<Light>());
  return debugLog("Engine", "loadSceneLighting", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadSceneTextures() {
  debugLog("Engine", "loadSceneTextures", "Start time: " + std::to_string(glfwGetTime()), true);
  return renderer.addTextures(sceneManager.getScene().getObjects<TextureData>()) 
    ? debugLog("Engine", "loadSceneTextures", "Finish time: " + std::to_string(glfwGetTime()), true)
    : error("Engine", "loadSceneTextures", "Failed to load scene textures");
}
bool Engine::loadSceneTextureConnections() {
  debugLog("Engine", "loadSceneTextureConnections", "Start time: " + std::to_string(glfwGetTime()), true);

  for (const std::pair<const std::string, TextureConnection>& data : sceneManager.getScene().getObjects<TextureConnection>()) {
    const TextureConnection& connection = data.second;
    if (connection.slot >= Model::NUM_TEXTURES) return error("Engine", "loadSceneTextureConnection", "Slot out of range: " + std::to_string(connection.slot));

    Model* model;
    if (!sceneManager.getScene().getObjectByName<Model>(connection.modelName, model))
      return error("Engine", "loadSceneTextureConnection", "Model " + connection.modelName + " not found for connection " + connection.name);

    if (connection.textureName.empty() || connection.mix <= 0.0f) {
      model->textureNames[connection.slot].clear();
      model->textureMixRatio[connection.slot] = 0.0f;

      bool any = false;
      for (unsigned int i = 0; i < Model::NUM_TEXTURES; ++i) {
        if (!model->textureNames[i].empty()) {
          any = true;
          break;
        }
      }

      model->useTextures = any;
      debugLog("Scene", "loadSceneTextureConnections", "unbind: " + connection.modelName + "[slot " + std::to_string(connection.slot) + "]", true);
      continue;
    }

    TextureData* texture;
    if (!sceneManager.getScene().getObjectByName<TextureData>(connection.textureName, texture))
      return error("Engine", "loadSceneTextureConnection", "Texture " + connection.textureName + " not found for connection " + connection.name);

    model->useTextures = true;
    model->textureNames[connection.slot] = connection.textureName;
    model->textureMixRatio[connection.slot] = (connection.mix < 0.0f) ? 0.0f : (connection.mix > 1.0f ? 1.0f : connection.mix);
  }
  return debugLog("Engine", "loadSceneTextureConnections", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadScenePrimitives() {
  debugLog("Engine", "loadScenePrimitives", "Start time: " + std::to_string(glfwGetTime()), true);

  for (std::pair<const std::string, Primitive>& data : sceneManager.getScene().getObjects<Primitive>()) {
    const Primitive& primitive = data.second;
    if (!renderer.createPrimitiveMesh(primitive))
      return error("Engine", "loadScenePrimitives", "Failed to create mesh for primitive: " + primitive.name);

    MeshGPU* primMesh;
    if (!renderer.getMesh(primitive.name, primMesh))
      return error("Engine", "loadScenePrimitives", "Failed to load primitive mesh: " + primitive.name);

    Model m{};
    m.name = primitive.name;
    m.meshPath = primitive.name;
    m.transform = primitive.transform;
    m.useTextures = false;
    for (unsigned i = 0; i < Model::NUM_TEXTURES; ++i) {
      m.textureNames[i].clear();
      m.textureMixRatio[i] = 0.0f;
    }
    m.colour = primitive.colour;
    m.colourMode = primitive.colourMode;
    if (!sceneManager.getScene().addObject<Model>(m, primitive.name.c_str()))
      return error("Engine", "loadScenePrimitives", "Failed to add primitive model: " + primitive.name);
  }

  return debugLog("Engine", "loadScenePrimitives", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadSceneGrids() {
  debugLog("Engine", "loadSceneGrids", "Start time: " + std::to_string(glfwGetTime()), true);

  for (std::pair<const std::string, Grid>& data : sceneManager.getScene().getObjects<Grid>()) {
    const Grid& grid = data.second;
    std::string sharedName = grid.name + (grid.type == GridType::Square ? "_sharedSquare" : "_sharedCube");

    if (!renderer.createGridMesh(grid, sharedName))
      return error("Engine", "loadSceneGrids", "Failed to create mesh for: " + sharedName);

    const int gridSide = (grid.count > 0) ? static_cast<int>(std::ceil(std::sqrt(static_cast<float>(grid.count)))) : 0;
    for (int i = 0; i < 0 + grid.count; ++i) {
      const int localIdx = i - 0;
      const int row = (gridSide > 0) ? (localIdx / gridSide) : 0;
      const int col = (gridSide > 0) ? (localIdx % gridSide) : 0;

      Vec3 pos{};
      if (grid.type == GridType::Square) {
        pos = { grid.spacing * static_cast<float>(col),
                grid.spacing * static_cast<float>(row),
                0.0f };
      }
      else {
        pos = { grid.spacing * static_cast<float>(col),
                0.0f,
                grid.spacing * static_cast<float>(row) };
      }

      Model m{};
      m.name = grid.name + "_instance_" + std::to_string(i);
      m.meshPath = sharedName;
      m.useTextures = false;
      for (unsigned ti = 0; ti < Model::NUM_TEXTURES; ++ti) {
        m.textureNames[ti].clear();
        m.textureMixRatio[ti] = 0.0f;
      }
      m.colour = grid.colour;
      m.colourMode = grid.colourMode;

      m.transform.pos = { pos, 1.0f };
      m.transform.rot = grid.transform.rot;
      m.transform.size = grid.transform.size;

      if (!sceneManager.getScene().addObject<Model>(m, m.name.c_str()))
        return error("Engine", "loadSceneGrids", "Failed to add grid instance model: " + m.name);
    }
  }

  return debugLog("Engine", "loadSceneGrids", "Finish time: " + std::to_string(glfwGetTime()), true);
}

#include "StarletEngine/engine.hpp"
#include <GLFW/glfw3.h>

bool Engine::loadScene(const std::string& sceneIn) {
  if (!sceneManager.loadTxtScene(assetPath + "/scenes/" + sceneIn + ".txt"))
    return error("Engine", "setScene", "Failed to load scene: " + sceneIn);

  if (sceneManager.scene.getScenePath().empty())
    if (!sceneManager.loadTxtScene(assetPath + "/scenes/Default.txt"))
      return error("Engine", "loadSceneMeshes", "No scene loaded and failed to load default scene");

  return loadSceneMeshes()
    && loadSceneLighting()
    && loadSceneTextures()
    && loadSceneTextureConnections()
    && loadScenePrimitives()
    && loadSceneGrids();
}

bool Engine::loadSceneMeshes() {
  debugLog("Engine", "loadSceneMeshes", "Start time: " + std::to_string(glfwGetTime()), true);

  meshManager.setBasePath(assetPath);
  for (const std::pair<const std::string, Model>& model : sceneManager.scene.getObjects<Model>())
    if (!meshManager.addMesh(model.second.meshPath))
      return error("Engine", "loadSceneMeshes", "Failed to load mesh: " + model.second.meshPath);

  return debugLog("Engine", "loadSceneMeshes", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadSceneLighting() {
  debugLog("Engine", "loadSceneLighting", "Start time: " + std::to_string(glfwGetTime()), true);

  if (renderer.getProgram() == 0) return error("Engine", "loadSceneLighting", "No active shader program set before loading lighting");

  renderer.updateLightCount(sceneManager.scene.getObjectCount<Light>());
  renderer.updateLightUniforms(sceneManager.scene.getObjects<Light>());

  return debugLog("Engine", "loadSceneLighting", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadSceneTextures() {
  debugLog("Engine", "loadSceneTextures", "Start time: " + std::to_string(glfwGetTime()), true);

  textureManager.setBasePath(assetPath);
  for (const std::pair<const std::string, TextureData>& data : sceneManager.scene.getObjects<TextureData>()) {
    const TextureData& texture = data.second;

    if (!texture.isCube) {
      if (!textureManager.addTexture(texture.name, texture.faces[0]))
        return error("Engine", "loadSceneTextures", "Failed to load 2D texture: " + texture.name);
    }
    else if (!textureManager.addCubeTexture(texture.name, texture.faces))
      return error("Engine", "loadSceneTextures", "Failed to load cube map: " + texture.name);
  }

  return debugLog("Engine", "loadSceneTextures", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadSceneTextureConnections() {
  debugLog("Engine", "loadSceneTextureConnections", "Start time: " + std::to_string(glfwGetTime()), true);

  for (const std::pair<const std::string, TextureConnection>& data : sceneManager.scene.getObjects<TextureConnection>()) {
    const TextureConnection& connection = data.second;
    if (connection.slot >= Model::NUM_TEXTURES) return error("Engine", "loadSceneTextureConnection", "Slot out of range: " + std::to_string(connection.slot));

    Model* model;
    if (!sceneManager.scene.getObjectByName<Model>(connection.modelName, model))
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
    if (!sceneManager.scene.getObjectByName<TextureData>(connection.textureName, texture))
      return error("Engine", "loadSceneTextureConnection", "Texture " + connection.textureName + " not found for connection " + connection.name);

    model->useTextures = true;
    model->textureNames[connection.slot] = connection.textureName;
    model->textureMixRatio[connection.slot] = (connection.mix < 0.0f) ? 0.0f : (connection.mix > 1.0f ? 1.0f : connection.mix);
  }
  return debugLog("Engine", "loadSceneTextureConnections", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadScenePrimitives() {
  debugLog("Engine", "loadScenePrimitives", "Start time: " + std::to_string(glfwGetTime()), true);

  for (std::pair<const std::string, Primitive>& data : sceneManager.scene.getObjects<Primitive>()) {
    const Primitive& primitive = data.second;

    bool ok = false;
    switch (primitive.type) {
    case PrimitiveType::Triangle:
      ok = meshManager.createTriangle(primitive.name, { primitive.transform.size.x, primitive.transform.size.y }, primitive.colour);
      break;
    case PrimitiveType::Square:
      ok = meshManager.createSquare(primitive.name, { primitive.transform.size.x, primitive.transform.size.y }, primitive.colour);
      break;
    case PrimitiveType::Cube:
      ok = meshManager.createCube(primitive.name, primitive.transform.size, primitive.colour);
      break;
    }
    if (!ok) return error("Engine", "loadScenePrimitives", "Failed to create primitive mesh: " + primitive.name);

    Mesh* primMesh;
    if (!meshManager.getMesh(primitive.name, primMesh))
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
    if (!sceneManager.scene.addObject<Model>(m, primitive.name.c_str()))
      return error("Engine", "loadScenePrimitives", "Failed to add primitive model: " + primitive.name);
  }

  return debugLog("Engine", "loadScenePrimitives", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadSceneGrids() {
  debugLog("Engine", "loadSceneGrids", "Start time: " + std::to_string(glfwGetTime()), true);

  for (std::pair<const std::string, Grid>& data : sceneManager.scene.getObjects<Grid>()) {
    const Grid& grid = data.second;

    std::string sharedName = grid.name + (grid.type == GridType::Square ? "_sharedSquare" : "_sharedCube");

    bool ok = false;
    switch (grid.type) {
    case GridType::Square:
      ok = meshManager.createSquare(sharedName, { grid.transform.size.x, grid.transform.size.y }, grid.colour);
      break;
    case GridType::Cube:
      ok = meshManager.createCube(sharedName, grid.transform.size, grid.colour);
      break;
    }

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

      if (!sceneManager.scene.addObject<Model>(m, m.name.c_str()))
        return error("Engine", "loadSceneGrids", "Failed to add grid instance model: " + m.name);
    }
  }

  return debugLog("Engine", "loadSceneGrids", "Finish time: " + std::to_string(glfwGetTime()), true);
}

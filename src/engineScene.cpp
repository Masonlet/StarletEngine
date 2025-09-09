#include "StarletEngine/engine.hpp"
#include <GLFW/glfw3.h>

bool Engine::loadScene(const std::string& sceneIn) {
  if (!sceneManager.loadTxtScene(assetPath + "/scenes/" + sceneIn + ".txt"))
    return error("Engine", "setScene", "Failed to load scene: " + sceneIn);

  if (sceneManager.scene.getScenePath().empty())
    if (!sceneManager.loadTxtScene(assetPath + "/scenes/Default.txt"))
      return error("Engine", "loadSceneMeshes", "No scene loaded and failed to load default scene");

  return loadSceneMeshes() && loadSceneLighting() && loadSceneTextures() && loadSceneTextureConnections();
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


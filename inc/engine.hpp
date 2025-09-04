#pragma once

#include "windowManager.hpp"
#include "renderer.hpp"
#include "starletgraphics/shaderManager.hpp"
#include "starletgraphics/meshManager.hpp"
#include "starletgraphics/textureManager.hpp"
#include "starletcontrols/inputManager.hpp"
#include "starletscene/sceneManager.hpp"
#include "starletcontrols/modelController.hpp"
#include "starletcontrols/cameraController.hpp"

struct Engine {
	WindowManager windowManager;

	Renderer renderer;
	ShaderManager shaderManager;
	MeshManager meshManager;
	TextureManager textureManager;

	InputManager inputManager;
	SceneManager sceneManager;

	ModelController modelController;
	FreeCameraController cameraController;

	bool wireframe{ false };
	float deltaTime{ 0.0f }, lastTime{ 0.0f };

	bool initialize(const unsigned int width, const unsigned int height, const char* title);
	bool setScene(const std::string& sceneIn = "Default") { return sceneManager.loadTxtScene(assetPath + sceneIn + ".txt"); }
	bool loadSceneAssets();

	void run();

private:
	std::string assetPath;

	bool setupShaders();
	bool loadSceneMeshes();
	bool loadSceneLighting();
	bool loadSceneTextures();
	bool loadSceneTextureConnections();

	void updateTime(const float currentTime);
	void renderFrame();
};

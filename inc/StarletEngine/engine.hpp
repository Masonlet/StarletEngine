#pragma once

#include "windowManager.hpp"
#include "renderer.hpp"
#include "StarletGraphics/shaderManager.hpp"
#include "StarletGraphics/meshManager.hpp"
#include "StarletGraphics/textureManager.hpp"
#include "StarletScene/sceneManager.hpp"
#include "StarletControls/inputManager.hpp"
#include "StarletControls/modelController.hpp"
#include "StarletControls/cameraController.hpp"

struct Engine {
	std::string assetPath;

	WindowManager windowManager;
	Renderer renderer;
	ShaderManager shaderManager;
	MeshManager meshManager;
	TextureManager textureManager;

	SceneManager sceneManager;

	InputManager inputManager;
	ModelController modelController;
	FreeCameraController cameraController;

	bool wireframe{ false };
	float deltaTime{ 0.0f }, lastTime{ 0.0f };

	bool initialize(const unsigned int width, const unsigned int height, const char* title);
	bool loadScene(const std::string& sceneIn = "Default");
	void run();

	Camera* getActiveCamera();

	void toggleCursorLock();

private:
	bool setupShaders();
	void setupGLState();

	bool loadSceneMeshes();
	bool loadSceneLighting();
	bool loadSceneTextures();
	bool loadSceneTextureConnections();
	bool loadScenePrimitives();
	bool loadSceneGrids();

	void updateTime(const float currentTime);
	void updateEngineState(Camera& cam);

	void handleKeyEvents(const std::vector<KeyEvent>& keyEvents);
	void handleScrollEvents(double xOffset, double yOffset);

	void renderFrame();
	void renderModels(const Vec3& eye);
	void renderSkybox(const Vec3& eye);
};

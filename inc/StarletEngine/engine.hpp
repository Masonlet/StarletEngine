#pragma once

#include "windowManager.hpp"
#include "StarletGraphics/renderer.hpp"
#include "StarletScene/sceneManager.hpp"
#include "StarletControls/inputManager.hpp"
#include "StarletControls/modelController.hpp"
#include "StarletControls/cameraController.hpp"

class Engine {
public:
	void setAssetPaths(const std::string& path);
	bool initialize(const unsigned int width, const unsigned int height, const char* title);
	bool loadScene(const std::string& sceneIn = "Default");
	void run();

	Camera* getActiveCamera();

	inline void updateViewport(const int width, const int height) { windowManager.updateViewport(width, height); }

	inline void onKey(const KeyEvent& event) { inputManager.onKey(event); }
	inline void onScroll(double xOffset, double yOffset) { inputManager.onScroll(xOffset, yOffset); }

	inline void toggleCursorLock() { inputManager.setCursorLocked(windowManager.switchCursorLock()); }
	inline void toggleWireframe() { renderer.toggleWireframe(); };

private:
	float deltaTime{ 0.0f }, lastTime{ 0.0f };

	Renderer renderer;
	WindowManager windowManager;
	SceneManager sceneManager;

	InputManager inputManager;
	ModelController modelController;
	FreeCameraController cameraController;

	bool setupShaders();
	void setupGLState();

	void updateTime(const float currentTime);
	void updateEngineState(Camera& cam);

	void handleKeyEvents(const std::vector<KeyEvent>& keyEvents);
	void handleScrollEvents(double xOffset, double yOffset);

	void renderFrame();
	void renderModels(const Vec3& eye);
	void renderSkybox(const Vec3& eye);

	bool loadSceneMeshes();
	bool loadSceneLighting();
	bool loadSceneTextures();
	bool loadSceneTextureConnections();
	bool loadScenePrimitives();
	bool loadSceneGrids();
};

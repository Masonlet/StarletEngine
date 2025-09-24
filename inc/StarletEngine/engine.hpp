#pragma once

#include "windowManager.hpp"
#include "timer.hpp"
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
	WindowManager windowManager;
	Renderer renderer;
	SceneManager sceneManager;

	InputManager inputManager;
	ModelController modelController;
	FreeCameraController cameraController;

	Timer timer;

	bool setupShaders();

	bool loadSceneTextureConnections();
	bool loadScenePrimitives();
	bool loadSceneGrids();

	void handleKeyEvents(const std::vector<KeyEvent>& keyEvents);
};

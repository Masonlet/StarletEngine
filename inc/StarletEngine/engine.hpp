#pragma once

#include "StarletEngine/windowManager.hpp"
#include "StarletEngine/timer.hpp"
#include "StarletControls/inputManager.hpp"

#include "StarletScene/sceneManager.hpp"

#include "StarletGraphics/loader/resourceLoader.hpp"
#include "StarletGraphics/manager/glStateManager.hpp"
#include "StarletGraphics/manager/resourceManager.hpp"
#include "StarletGraphics/renderer/renderer.hpp"

class Engine {
public:
	Engine();
	~Engine() = default;

	void setAssetPaths(const std::string& path);
	bool initialize(const unsigned int width, const unsigned int height, const char* title);

	bool loadScene(const std::string& sceneIn = "Default");
	void run();

	void updateViewport(const int width, const int height) { windowManager.updateViewport(width, height); }

	void onKey(const KeyEvent& event) { inputManager.onKey(event); }
	void onScroll(double xOffset, double yOffset) { inputManager.onScroll(xOffset, yOffset); }
	void onButton(const MouseButtonEvent& event) { inputManager.onButton(event); }

	void toggleCursorLock() { inputManager.setCursorLocked(windowManager.switchCursorLock()); }
	void toggleWireframe() { glState.toggleWireframe(); };

private:

	WindowManager windowManager;
	Timer timer;
	InputManager inputManager;
	GLStateManager glState;

	SceneManager sceneManager;
	ResourceManager resourceManager;
	ResourceLoader resourceLoader;

	Renderer renderer;

	void handleKeyEvents(const std::vector<KeyEvent>& keyEvents);
	void handleButtonEvents(const std::vector<MouseButtonEvent>& buttonEvents);
};

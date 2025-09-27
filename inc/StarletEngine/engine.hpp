#pragma once

#include "windowManager.hpp"
#include "timer.hpp"
#include "resourceLoader.hpp"
#include "StarletGraphics/renderer.hpp"
#include "StarletScene/sceneManager.hpp"
#include "StarletControls/inputManager.hpp"

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
	void toggleWireframe() { renderer.toggleWireframe(); };

private:
	WindowManager windowManager;
	Timer timer;
	InputManager inputManager;

	ShaderManager shaderManager;
	MeshManager meshManager;
	TextureManager textureManager;

	Renderer renderer;
	ResourceLoader resourceLoader;
	SceneManager sceneManager;

	void handleKeyEvents(const std::vector<KeyEvent>& keyEvents);
	void handleButtonEvents(const std::vector<MouseButtonEvent>& buttonEvents);
};

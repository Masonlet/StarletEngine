#pragma once

#include "windowManager.hpp"
#include "StarletGraphics/renderer.hpp"
#include "StarletGraphics/shader/shaderManager.hpp"
#include "StarletGraphics/mesh/meshManager.hpp"
#include "StarletGraphics/texture/textureManager.hpp"
#include "StarletScene/sceneManager.hpp"
#include "StarletControls/inputManager.hpp"
#include "StarletControls/modelController.hpp"
#include "StarletControls/cameraController.hpp"

class Engine {
public:
	inline void setAssetPath(const std::string& path) { assetPath = path; }
	const std::string& getAssetPath() const { return assetPath; }

	bool initialize(const unsigned int width, const unsigned int height, const char* title);
	bool loadScene(const std::string& sceneIn = "Default");
	void run();

	Camera* getActiveCamera();

	inline void updateViewport(const int width, const int height) { windowManager.updateViewport(width, height); }

	inline void onKey(const KeyEvent& event) { inputManager.onKey(event); }
	inline void onScroll(double xOffset, double yOffset) { inputManager.onScroll(xOffset, yOffset); }

	void toggleCursorLock();
	void toggleWireframe();

private:
	std::string assetPath;
	bool wireframe{ false };
	float deltaTime{ 0.0f }, lastTime{ 0.0f };

	Renderer renderer;
	WindowManager windowManager;
	SceneManager sceneManager;

	ShaderManager shaderManager;
	MeshManager meshManager;
	TextureManager textureManager;

	InputManager inputManager;
	ModelController modelController;
	FreeCameraController cameraController;

	void setAssetPaths();
	bool setupShaders();
	void setupGLState();

	void updateTime(const float currentTime);
	void updateEngineState(Camera& cam);

	void handleKeyEvents(const std::vector<KeyEvent>& keyEvents);
	void handleScrollEvents(double xOffset, double yOffset);

	void renderFrame();
	void renderModels(const Vec3& eye);
	void renderSkybox(const Vec3& eye);

	// To be moved.. maybe?
	bool loadSceneMeshes();
	bool loadSceneLighting();
	bool loadSceneTextures();
	bool loadSceneTextureConnections();
	bool loadScenePrimitives();
	bool loadSceneGrids();
};

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

class Engine {
public:
	InputManager inputManager;
	WindowManager windowManager;

	inline void setAssetPath(const std::string& path) { assetPath = path; }
	const std::string& getAssetPath() const { return assetPath; }

	bool initialize(const unsigned int width, const unsigned int height, const char* title);
	bool loadScene(const std::string& sceneIn = "Default");
	void run();

	Camera* getActiveCamera();
	void toggleCursorLock();
	void toggleWireframe();

private:
	std::string assetPath;
	bool wireframe{ false };
	float deltaTime{ 0.0f }, lastTime{ 0.0f };

	Renderer renderer;
	ShaderManager shaderManager;
	MeshManager meshManager;
	TextureManager textureManager;
	SceneManager sceneManager;
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

#include <glad/glad.h>
#include "StarletEngine/windowManager.hpp"
#include "StarletEngine/callbacks.hpp"
#include "StarletParsers/utils/log.hpp"
#include "StarletControls/inputManager.hpp"
#include <cstdio>

constexpr int GL_MAJOR{ 3 };
constexpr int GL_MINOR{ 3 };

WindowManager::WindowManager() : window(nullptr) {
  if (!glfwInit()) error("WindowManager", "Constructor", "Failed to initialize GLFW\n");
}

WindowManager::~WindowManager() {
  destroyWindow();
  glfwTerminate();
}
void WindowManager::pollEvents() const {
  if (window) window->pollEvents();
}
void WindowManager::swapBuffers() const {
  if (window) window->swapBuffers();
}
bool WindowManager::shouldClose() const {
  return window ? window->shouldClose() : true;
}
bool WindowManager::createWindow(const unsigned int width, const unsigned int height, const char* title) {
  debugLog("WindowManager", "createWindow", "Start time : " + std::to_string(glfwGetTime()), true);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  window = new Window();
  if (!window->createWindow(width, height, title)) {
    delete window;
    window = nullptr;
    return error("WindowManager", "createWindow", "OS window creation failed");
  }

  glfwMakeContextCurrent(window->getGLFWwindow());
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    delete window;
    window = nullptr;
    return error("WindowManager", "createWindow", "Failed to initialize GLAD");
  }

  glfwSetKeyCallback(window->getGLFWwindow(), key_callback);
  glfwSetFramebufferSizeCallback(window->getGLFWwindow(), framebuffer_size_callback);
  glfwSetScrollCallback(window->getGLFWwindow(), scroll_callback);
  glfwSetInputMode(window->getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSwapInterval(1);

  setInitialViewport();

  debugLog("Window", "OpenGL", "OpenGL Info", true);
  debugLog("Window", "OpenGL", "Version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))), true);
  debugLog("Window", "OpenGL", "Vendor: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR))), true);
  debugLog("Window", "OpenGL", "Renderer: " + std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER))), true);
  return debugLog("WindowManager", "createWindow", "Finish time: " + std::to_string(glfwGetTime()), true);
}
void WindowManager::destroyWindow() {
  delete window;
  window = nullptr;
}

void WindowManager::setInitialViewport() {
  int w = 0, h = 0;
  glfwGetFramebufferSize(window->getGLFWwindow(), &w, &h);
  if (w <= 0 || h <= 0) {
    w = static_cast<int>(window->getWidth());
    h = static_cast<int>(window->getHeight());
  }
  glViewport(0, 0, w, h);
}

void WindowManager::switchActiveWindowVisibility() {
  if (window) glfwGetWindowAttrib(getWindow()->getGLFWwindow(), GLFW_VISIBLE)
    ? glfwHideWindow(getWindow()->getGLFWwindow())
    : glfwShowWindow(getWindow()->getGLFWwindow());
  else debugLog("WindowManager", "switchActiveWindowVisibility", "No active window to switch visibility.");
}

void WindowManager::updateInput(InputManager& inputManager) {
  if (window) inputManager.update(window->getGLFWwindow());
}

void WindowManager::requestClose() {
  if (window) glfwSetWindowShouldClose(window->getGLFWwindow(), GLFW_TRUE);
}

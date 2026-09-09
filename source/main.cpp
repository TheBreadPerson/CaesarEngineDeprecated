#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <memory>
#include "renderer.hpp"
#include "inputs.hpp"
#include "camera.hpp"
#include "globals.hpp"
#include "scene.hpp"
#include "gamescene.hpp"
#include "editor.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GLFWwindow* window;
Renderer renderer;
Camera cam;
Player player;
Editor editor;

SceneManager sceneManager;
std::shared_ptr<GameScene> gameScene = std::make_shared<GameScene>();

unsigned int screen_width = 1500;
unsigned int screen_height = 844;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    screen_width = width;
    screen_height = height;
}

void init()
{
    // GLFW Initialization
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Error handling
    window = glfwCreateWindow(screen_width, screen_height, "Caesar Engine", NULL, NULL);
    if (window == nullptr)
    {
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    renderer.init();
    sceneManager.openScene(gameScene);
    editor.init();
    cam.init();
    player.init();
}

void GameLoop()
{
    gameScene->Update();
}

void FixedGameLoop(double deltaTime)
{
    gameScene->FixedUpdate(deltaTime);
}

int main()
{
    init();
    while (!glfwWindowShouldClose(window))
    {
        Time::Tick();
        if (Input::GetKeyDown(KeyCode::ESC))
        {
            glfwSetWindowShouldClose(window, true);
        }

        GameLoop();
        FixedGameLoop(Time::deltaTime);

        // input
        Input::processInput();

        if (Input::GetKeyDown(KeyCode::P))
        {
            glfwSetInputMode(window, GLFW_CURSOR, (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
        }

        float x, y;
        Input::GetMouse(x, y);
        ImGui_ImplGlfw_CursorPosCallback(window, x, y);

        
        // rendering
        editor.draw();
        renderer.draw(window);
    }
    renderer.cleanup();
    glfwTerminate();

    return 0;
}
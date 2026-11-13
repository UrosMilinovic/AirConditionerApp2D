#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include "Config.h"
#include "Util.h"
#include "Game.h"
#include "Render.h"
#include "Input.h"


int endProgram(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

int main()
{
    std::srand((unsigned)std::time(nullptr));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = FULLSCREEN ? glfwGetPrimaryMonitor() : NULL;
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int winW = FULLSCREEN ? mode->width : 1280;
    int winH = FULLSCREEN ? mode->height : 720;

    GLFWwindow* window = glfwCreateWindow(winW, winH, "Klima uredjaj", monitor, NULL);
    if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); 

    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);

    if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);

    buildLayout((float)fbW / (float)fbH);
    initRender();

    GLFWcursor* cursor = loadImageToCursor("res/cursor_remote.png", 5, 5);
    if (cursor) glfwSetCursor(window, cursor);

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        double frameStart = glfwGetTime();
        float dt = (float)(frameStart - lastTime); 
        lastTime = frameStart;
        if (dt > 0.1f) dt = 0.1f;                  

        update(dt);

        glClear(GL_COLOR_BUFFER_BIT);
        drawScene();

        glfwSwapBuffers(window);
        glfwPollEvents();

        while (glfwGetTime() < frameStart + FRAME_TIME) {}
    }

    cleanupRender();
    if (cursor) glfwDestroyCursor(cursor);
    glfwTerminate();
    return 0;
}
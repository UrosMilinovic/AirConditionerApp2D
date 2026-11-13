#include "Input.h"
#include "Game.h"

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    // PRESS i REPEAT, da drzanje tastera menja temperaturu
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_UP)   changeTarget(+1);
        if (key == GLFW_KEY_DOWN) changeTarget(-1);
    }

    // Prazni lavor
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        emptyBasin();
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float x = ((float)(xpos / w) * 2.0f - 1.0f) * aspect;
        float y = -((float)(ypos / h) * 2.0f - 1.0f);

        if (hitLamp(x, y))
            toggleAC();
    }
}
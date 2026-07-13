#include "gui.h"
#include <iostream>

GLFWwindow* GuiManager::guiWindowSetUp() {

    // init glfw
    if (!glfwInit()) {
        throw ErrorObj{ GLFW_FAILED_INIT , "glfw failed init"};
    }

    // give info to window creations
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create window
    GLFWwindow* window = glfwCreateWindow(800, 800, "Tem's Chess", NULL, NULL);
    if (window == NULL) {
        
        ErrorObj error{ WINDOW_FAILED_INIT, "glfw window failed to create" };
        throw error;

    }

    return window;

}

void GuiManager::guiWindowCleanUp(GLFWwindow* window){

    if (window != nullptr) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();

}

int GuiManager::guiMainLoop()
{
    GLFWwindow* window = nullptr;

    // window pbject should be successfuly pointed to by window 
    try {
        window = guiWindowSetUp();
    }
    catch (const ErrorObj& error) {

        guiWindowCleanUp(window);
        std::cout << error.error_msg << std::endl;
        return -1;
    }

    // make openGl context connected to our window
    glfwMakeContextCurrent(window);

    // use glad to load openGl functions
    gladLoadGL();
    glViewport(0, 0, 800, 800);

    glClearColor(0.42f, 0.13f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    //--------------------------------------------

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();



    }

    //clean up
    guiWindowCleanUp(window);
    return 0;
}
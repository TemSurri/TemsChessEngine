#include "gui.h"
#include <iostream>

const char* vertexShaderSource = R"(#version 330 core

layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.2, 0.8, 0.3, 1.0);
}
)";

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

    //draw mat
    GLfloat vertices[] = {

        -0.5f, -0.1f, 0.0f,

        -0.3f, -0.8f, 0.0f,

        -0.1f, -0.2f, 0.0f

    };

    //setting up shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //buffers = big batches of data
    //creating buffers

    GLuint VAO, VBO;

    //VAO is a way to show how to use the data stored in VBO
    glGenVertexArrays(1, &VAO);

    //VBO is a way to simpley story the data in a state openGl can use
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    while (!glfwWindowShouldClose(window)) {
        
        glClearColor(0.42f, 0.13f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);



        glfwSwapBuffers(window);



        glfwPollEvents();



    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);


    //clean up window
    guiWindowCleanUp(window);
    return 0;
}
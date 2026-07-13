#include "gui.h"
#include <iostream>
#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"

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

        -0.2f, 0.2f, 0.0f, 0.0f, 0.0f, 0.5f,
        -0.2f, -0.8f, 0.0f, 0.0f, 0.5f, 0.5f,
        0.8f, 0.2f, 0.0f, 0.0f, 0.0f, 0.5f,
        0.8f, -0.8f, 0.0f, 0.5f, 0.0f, 0.5f,


    };

    GLuint indices[] = {
        0, 1, 3,
        0, 2, 3

    };


    VAO vao1;
    vao1.Bind();

    VBO vbo1(vertices, sizeof(vertices));
  
    EBO ebo1(indices, sizeof(indices));


    vao1.LinkAttrib(vbo1, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0);
    vao1.LinkAttrib(vbo1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3*sizeof(float)));
    vao1.Unbind();
    vbo1.Unbind();
    ebo1.Unbind();

    //setting up shaders
    try
    {
        Shader shaderProgram(
            "resources/shaders/default.vert",
            "resources/shaders/default.frag"
        );

        shaderProgram.Activate();


        while (!glfwWindowShouldClose(window)) {

            
            shaderProgram.Activate();
            glClearColor(0.42f, 0.13f, 0.11f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            vao1.Bind();


            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


            glfwSwapBuffers(window);


            glfwPollEvents();



        }

        shaderProgram.Delete();
        vao1.Delete();
        vbo1.Delete();
        ebo1.Delete();





    }




    catch (const Shader::ErrorObj& error)
    {
        std::cerr << "Shader error: " << error.error_msg << '\n';
        guiWindowCleanUp(window);
        return -1;
    }
    //clean up window
    guiWindowCleanUp(window);
    return 0;
}
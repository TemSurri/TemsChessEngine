#include "gui.h"
#include <iostream>
#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "../external/stb/stb_image.h"



//getting vertices

std::vector<GuiManager::colored_vertex>
GuiManager::getVerticesForBoard()
{
    std::vector<colored_vertex> vertices;
    vertices.reserve(64 * 4);

    constexpr float boardStart = -0.8f;
    constexpr float squareSize = 0.2f;

    const Color light{ 0.85f, 0.78f, 0.65f };
    const Color dark{ 0.35f, 0.18f, 0.10f };

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            float x = boardStart + col * squareSize;
            float y = boardStart + row * squareSize;

            Color color =
                ((row + col) % 2 == 0)
                ? light
                : dark;

            vertices.insert(vertices.end(), {
                {x,              y,              0.0f, color.r, color.g, color.b},
                {x + squareSize, y,              0.0f, color.r, color.g, color.b},
                {x + squareSize, y + squareSize, 0.0f, color.r, color.g, color.b},
                {x,              y + squareSize, 0.0f, color.r, color.g, color.b}
                });
        }
    }

    return vertices;
}

std::vector< GuiManager::pieceText> GuiManager::getVerticesForPieces(ClassicChess& game) {

    std::vector< GuiManager::pieceText> v;

    return v;

};


std::vector<GLuint> GuiManager::getBoardIndices()
{
    std::vector<GLuint> indices;
    indices.reserve(64 * 6);

    for (GLuint square = 0; square < 64; square++)
    {
        GLuint base = square * 4;

        indices.insert(indices.end(), {
            base, base + 1, base + 2,
            base, base + 2, base + 3
            });
    }

    return indices;
}

// little set up
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

//window clean up
void GuiManager::guiWindowCleanUp(GLFWwindow* window){

    if (window != nullptr) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();

}

//change the size of the viewport window automatically
void GuiManager::framebufferSizeCallback(
    GLFWwindow* window,
    int width,
    int height)
{
    // Choose the smaller dimension so the viewport remains square.
    int viewportSize = std::min(width, height);

    // Center the square viewport.
    int viewportX = (width - viewportSize) / 2;
    int viewportY = (height - viewportSize) / 2;

    glViewport(
        viewportX,
        viewportY,
        viewportSize,
        viewportSize
    );
}

// used to convert mouse hit coord to sqaure
int GuiManager::mouseToSquare(GLFWwindow* window)
{
    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    // Same calculation used by the responsive viewport.
    float viewportSize =
        static_cast<float>(std::min(width, height));

    float viewportX = (width - viewportSize) / 2.0f;
    float viewportY = (height - viewportSize) / 2.0f;

    // Board occupies -0.8 to 0.8, or 80% of viewport.
    float boardSize = viewportSize * 0.8f;

    // Remaining 20% gives a 10% margin on each side.
    float boardLeft = viewportX + viewportSize * 0.1f;
    float boardTop = viewportY + viewportSize * 0.1f;

    float squareSize = boardSize / 8.0f;

    // Check whether click is inside the board.
    if (mouseX < boardLeft ||
        mouseX >= boardLeft + boardSize ||
        mouseY < boardTop ||
        mouseY >= boardTop + boardSize)
    {
        return -1;
    }

    int column = static_cast<int>(
        (mouseX - boardLeft) / squareSize
        );

    // GLFW mouse Y begins at the top.
    int rowFromTop = static_cast<int>(
        (mouseY - boardTop) / squareSize
        );

    // Bitboard row 0 is usually at the bottom.
    int row = 7 - rowFromTop;

    return row * 8 + column;
}


//chess atlas texture set up
GLuint loadTexture(const char* path)
{
    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_set_flip_vertically_on_load(false);

    unsigned char* data = stbi_load(
        path,
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );

    if (!data)
    {
        std::cerr
            << "Failed to load texture: "
            << path << '\n'
            << stbi_failure_reason() << '\n';

        return 0;
    }

    GLuint textureID = 0;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
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
    glfwSetFramebufferSizeCallback(
        window,
        &GuiManager::framebufferSizeCallback
    );
    // use glad to load openGl functions
    gladLoadGL();
    //--------------------------------------------

    // texture load
    //GLuint textID = loadTexture("/resources/textures/chess_atlas.png");



    ClassicChess game;

    auto indicesBoard = getBoardIndices();
    auto verticesBoard = getVerticesForBoard();
    
    VAO vaoBoard;
    vaoBoard.Bind();

    VBO vboBoard(
        verticesBoard.data(),
        static_cast<GLsizeiptr>(
            verticesBoard.size() * sizeof(colored_vertex)
            )
    );

    EBO eboBoard(
        indicesBoard.data(),
        static_cast<GLsizeiptr>(
            indicesBoard.size() * sizeof(GLuint)
            )
    );
    
    vaoBoard.Bind();
    vaoBoard.LinkAttrib(
        vboBoard,
        0, // shader location
        3, // x, y, z
        GL_FLOAT,
        sizeof(colored_vertex),
        reinterpret_cast<void*>(
            offsetof(colored_vertex, x)
            )
    );

    vaoBoard.LinkAttrib(
        vboBoard,
        1, // shader location
        3, // r, g, b
        GL_FLOAT,
        sizeof(colored_vertex),
        reinterpret_cast<void*>(
            offsetof(colored_vertex, r)
            )
    );

    

    vaoBoard.Unbind();
    vboBoard.Unbind();
    eboBoard.Unbind();
    

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
            

            glClearColor(0.82f, 0.76f, 0.71f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);




            vaoBoard.Bind();

            glDrawElements(
                GL_TRIANGLES,
                static_cast<GLsizei>(indicesBoard.size()),
                GL_UNSIGNED_INT,
                nullptr
            );

            glfwSwapBuffers(window);
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                int square = mouseToSquare(window);

                if (square != -1)
                {
                    std::cout << "Clicked square: " << square << '\n';
                }
            }

            glfwPollEvents();

        }

        vboBoard.Delete();
        vaoBoard.Delete();
        shaderProgram.Delete();

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
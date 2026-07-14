#include "gui.h"

#include "../external/stb/stb_image.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace
{
    constexpr float BOARD_START = -0.8f;
    constexpr float SQUARE_SIZE = 0.2f;

    GLuint loadTexture(const char* path)
    {
        int width = 0;
        int height = 0;
        int channels = 0;

        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(
            path,
            &width,
            &height,
            &channels,
            STBI_rgb_alpha
        );

        if (data == nullptr)
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
}

// -----------------------------------------------------------------------------
// Game interaction
// -----------------------------------------------------------------------------

bool GuiManager::isAITurn(
    const ClassicChess& game) const
{
    if (gameMode != GameMode::PLAYER_VS_AI)
    {
        return false;
    }

    return game.isWhiteTurn() == aiPlaysWhite;
}

bool GuiManager::selectedPieceBelongsToCurrentPlayer(
    const ClassicChess& game,
    int square) const
{
    const ClassicChess::PieceTypeBit piece =
        game.piece_on_square(square);

    if (piece == ClassicChess::NO_PIECE)
    {
        return false;
    }

    const bool pieceIsWhite =
        piece >= ClassicChess::W_PAWN &&
        piece <= ClassicChess::W_KING;

    return pieceIsWhite == game.isWhiteTurn();
}

bool GuiManager::checkAndLogGameOver(
    ClassicChess& game)
{
    if (gameFinished)
    {
        return true;
    }

    const ClassicChess::OutCome outcome =
        game.getGameState();

    if (outcome == ClassicChess::Normal)
    {
        return false;
    }

    gameFinished = true;
    aiMovePending = false;
    selectedSquare = -1;

    std::cout << "\n========================================\n";

    switch (outcome)
    {
    case ClassicChess::WhiteWin:
        std::cout << "GAME OVER: White wins by checkmate.\n";
        break;

    case ClassicChess::BlackWin:
        std::cout << "GAME OVER: Black wins by checkmate.\n";
        break;

    case ClassicChess::Draw:
        std::cout << "GAME OVER: Draw by stalemate.\n";
        break;

    case ClassicChess::Normal:
        break;
    }

    std::cout << "Close the window to exit.\n";
    std::cout << "========================================\n\n";

    return true;
}

void GuiManager::rebuildPieces(
    ClassicChess& game)
{
    PieceVAO.Delete();
    PieceVBO.Delete();
    PieceEBO.Delete();

    initializePieces(game);
}

void GuiManager::processMouseInput(
    GLFWwindow* window,
    ClassicChess& game)
{
    if (gameFinished)
    {
        return;
    }

    if (isAITurn(game))
    {
        selectedSquare = -1;
        return;
    }

    const bool leftMousePressed =
        glfwGetMouseButton(
            window,
            GLFW_MOUSE_BUTTON_LEFT
        ) == GLFW_PRESS;

    const bool newClick =
        leftMousePressed && !leftMouseWasPressed;

    leftMouseWasPressed = leftMousePressed;

    if (!newClick)
    {
        return;
    }

    const int clickedSquare =
        mouseToSquare(window);

    if (clickedSquare == -1)
    {
        selectedSquare = -1;
        return;
    }

    if (selectedSquare == -1)
    {
        if (selectedPieceBelongsToCurrentPlayer(
            game,
            clickedSquare
        ))
        {
            selectedSquare = clickedSquare;

            std::cout
                << "Selected square "
                << selectedSquare
                << ".\n";
        }

        return;
    }

    if (clickedSquare == selectedSquare)
    {
        selectedSquare = -1;
        return;
    }

    if (selectedPieceBelongsToCurrentPlayer(
        game,
        clickedSquare
    ))
    {
        selectedSquare = clickedSquare;
        return;
    }

    const int from = selectedSquare;
    const int to = clickedSquare;

    if (!game.tryMove(from, to))
    {
        std::cout << "Illegal move.\n";
        selectedSquare = -1;
        return;
    }

    rebuildPieces(game);

    std::cout
        << "Human move: "
        << from
        << " -> "
        << to
        << '\n';

    selectedSquare = -1;

    if (checkAndLogGameOver(game))
    {
        return;
    }

    // The human position must be presented before the AI changes the board.
    waitOneFrameBeforeAI = true;
    aiMovePending = true;
}

void GuiManager::processAITurn(
    ClassicChess& game)
{
    if (gameFinished)
    {
        return;
    }

    if (!isAITurn(game))
    {
        aiMovePending = true;
        return;
    }

    if (waitOneFrameBeforeAI)
    {
        waitOneFrameBeforeAI = false;
        return;
    }

    if (!aiMovePending)
    {
        return;
    }

    if (checkAndLogGameOver(game))
    {
        return;
    }

    aiMovePending = false;

    std::cout
        << "AI searching at depth "
        << aiDepth
        << "...\n";

    if (!game.makeAIMove(aiDepth))
    {
        checkAndLogGameOver(game);
        return;
    }

    rebuildPieces(game);

    std::cout << "AI move completed.\n";

    checkAndLogGameOver(game);
}

// -----------------------------------------------------------------------------
// OpenGL initialization
// -----------------------------------------------------------------------------

void GuiManager::initializeBoard()
{
    auto vertices = getVerticesForBoard();
    auto indices = getBoardIndices();

    boardIndexCount =
        static_cast<GLsizei>(indices.size());

    VAO vao;
    vao.Create();
    vao.Bind();

    VBO vbo(
        vertices.data(),
        static_cast<GLsizeiptr>(
            vertices.size() * sizeof(colored_vertex)
            )
    );

    EBO ebo(
        indices.data(),
        static_cast<GLsizeiptr>(
            indices.size() * sizeof(GLuint)
            )
    );

    vao.LinkAttrib(
        vbo,
        0,
        3,
        GL_FLOAT,
        sizeof(colored_vertex),
        reinterpret_cast<void*>(
            offsetof(colored_vertex, x)
            )
    );

    vao.LinkAttrib(
        vbo,
        1,
        3,
        GL_FLOAT,
        sizeof(colored_vertex),
        reinterpret_cast<void*>(
            offsetof(colored_vertex, r)
            )
    );

    vao.Unbind();
    vbo.Unbind();

    BoardVAO = std::move(vao);
    BoardVBO = std::move(vbo);
    BoardEBO = std::move(ebo);
}

void GuiManager::initializePieces(
    ClassicChess& game)
{
    auto pieceVertices =
        getVerticesForPieces(game);

    auto pieceIndices =
        getPieceIndices(pieceVertices.size() / 4);

    pieceIndexCount =
        static_cast<GLsizei>(pieceIndices.size());

    VAO pieceVAO;
    pieceVAO.Create();
    pieceVAO.Bind();

    VBO pieceVBO(
        pieceVertices.data(),
        static_cast<GLsizeiptr>(
            pieceVertices.size() * sizeof(textured_vertex)
            )
    );

    EBO pieceEBO(
        pieceIndices.data(),
        static_cast<GLsizeiptr>(
            pieceIndices.size() * sizeof(GLuint)
            )
    );

    pieceVAO.LinkAttrib(
        pieceVBO,
        0,
        3,
        GL_FLOAT,
        sizeof(textured_vertex),
        reinterpret_cast<void*>(
            offsetof(textured_vertex, x)
            )
    );

    pieceVAO.LinkAttrib(
        pieceVBO,
        1,
        2,
        GL_FLOAT,
        sizeof(textured_vertex),
        reinterpret_cast<void*>(
            offsetof(textured_vertex, u)
            )
    );

    pieceVAO.Unbind();
    pieceVBO.Unbind();
    pieceEBO.Unbind();

    PieceVAO = std::move(pieceVAO);
    PieceVBO = std::move(pieceVBO);
    PieceEBO = std::move(pieceEBO);
}

void GuiManager::initializeShaders()
{
    Shader boardProgram(
        "resources/shaders/default.vert",
        "resources/shaders/default.frag"
    );

    Shader pieceProgram(
        "resources/shaders/piece.vert",
        "resources/shaders/piece.frag"
    );

    boardShader = std::move(boardProgram);
    pieceShader = std::move(pieceProgram);

    pieceShader.Activate();

    glUniform1i(
        glGetUniformLocation(
            pieceShader.ID,
            "chessAtlas"
        ),
        0
    );
}

void GuiManager::initializeTexture()
{
    atlasTexture = loadTexture(
        "resources/textures/chess_atlas.png"
    );

    if (atlasTexture == 0)
    {
        throw std::runtime_error(
            "Failed to load chess atlas"
        );
    }

    glEnable(GL_BLEND);
    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );
}

// -----------------------------------------------------------------------------
// CPU-side mesh generation
// -----------------------------------------------------------------------------

std::vector<GuiManager::colored_vertex>
GuiManager::getVerticesForBoard()
{
    std::vector<colored_vertex> vertices;
    vertices.reserve(64 * 4);

    const Color light{
        0.85f,
        0.78f,
        0.65f
    };

    const Color dark{
        0.35f,
        0.18f,
        0.10f
    };

    for (int row = 0; row < 8; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            const float x =
                BOARD_START + column * SQUARE_SIZE;

            const float y =
                BOARD_START + row * SQUARE_SIZE;

            const Color color =
                ((row + column) % 2 == 0)
                ? light
                : dark;

            vertices.insert(
                vertices.end(),
                {
                    {x, y, 0.0f, color.r, color.g, color.b},
                    {x + SQUARE_SIZE, y, 0.0f, color.r, color.g, color.b},
                    {x + SQUARE_SIZE, y + SQUARE_SIZE, 0.0f, color.r, color.g, color.b},
                    {x, y + SQUARE_SIZE, 0.0f, color.r, color.g, color.b}
                }
            );
        }
    }

    return vertices;
}

std::vector<GLuint>
GuiManager::getBoardIndices()
{
    std::vector<GLuint> indices;
    indices.reserve(64 * 6);

    for (GLuint square = 0; square < 64; square++)
    {
        const GLuint base = square * 4;

        indices.insert(
            indices.end(),
            {
                base,
                base + 1,
                base + 2,

                base,
                base + 2,
                base + 3
            }
        );
    }

    return indices;
}

GuiManager::AtlasCell GuiManager::getAtlasCell(
    ClassicChess::PieceTypeBit piece)
{
    const bool isWhite =
        piece >= ClassicChess::W_PAWN &&
        piece <= ClassicChess::W_KING;

    const int row = isWhite ? 0 : 1;
    int column = -1;

    switch (piece)
    {
    case ClassicChess::W_KING:
    case ClassicChess::B_KING:
        column = 0;
        break;

    case ClassicChess::W_QUEEN:
    case ClassicChess::B_QUEEN:
        column = 1;
        break;

    case ClassicChess::W_ROOK:
    case ClassicChess::B_ROOK:
        column = 2;
        break;

    case ClassicChess::W_BISHOP:
    case ClassicChess::B_BISHOP:
        column = 3;
        break;

    case ClassicChess::W_KNIGHT:
    case ClassicChess::B_KNIGHT:
        column = 4;
        break;

    case ClassicChess::W_PAWN:
    case ClassicChess::B_PAWN:
        column = 5;
        break;

    default:
        return { -1, -1 };
    }

    return { column, row };
}

GuiManager::UVRegion GuiManager::getUVRegion(
    int column,
    int row)
{
    const float uMin =
        column * CELL_WIDTH;

    const float uMax =
        (column + 1) * CELL_WIDTH;

    const float vMax =
        1.0f - row * CELL_HEIGHT;

    const float vMin =
        1.0f - (row + 1) * CELL_HEIGHT;

    return {
        uMin,
        vMin,
        uMax,
        vMax
    };
}

std::vector<GuiManager::textured_vertex>
GuiManager::getVerticesForPieces(
    ClassicChess& game)
{
    std::vector<textured_vertex> vertices;
    vertices.reserve(32 * 4);

    for (int square = 0; square < 64; square++)
    {
        const ClassicChess::PieceTypeBit piece =
            game.piece_on_square(square);

        if (piece == ClassicChess::NO_PIECE)
        {
            continue;
        }

        const int row = square / 8;
        const int column = square % 8;

        const float x =
            BOARD_START + column * SQUARE_SIZE;

        const float y =
            BOARD_START + row * SQUARE_SIZE;

        const AtlasCell cell =
            getAtlasCell(piece);

        const UVRegion uv =
            getUVRegion(cell.column, cell.row);

        vertices.insert(
            vertices.end(),
            {
                {x, y, 0.0f, uv.uMin, uv.vMin},
                {x + SQUARE_SIZE, y, 0.0f, uv.uMax, uv.vMin},
                {x + SQUARE_SIZE, y + SQUARE_SIZE, 0.0f, uv.uMax, uv.vMax},
                {x, y + SQUARE_SIZE, 0.0f, uv.uMin, uv.vMax}
            }
        );
    }

    return vertices;
}

std::vector<GLuint>
GuiManager::getPieceIndices(
    std::size_t pieceCount)
{
    std::vector<GLuint> indices;
    indices.reserve(pieceCount * 6);

    for (
        GLuint piece = 0;
        piece < static_cast<GLuint>(pieceCount);
        piece++
        )
    {
        const GLuint base = piece * 4;

        indices.insert(
            indices.end(),
            {
                base,
                base + 1,
                base + 2,

                base,
                base + 2,
                base + 3
            }
        );
    }

    return indices;
}

// -----------------------------------------------------------------------------
// Window and mouse handling
// -----------------------------------------------------------------------------

GLFWwindow* GuiManager::guiWindowSetUp()
{
    if (!glfwInit())
    {
        throw ErrorObj{
            GLFW_FAILED_INIT,
            "GLFW failed to initialize"
        };
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window = glfwCreateWindow(
        VIEWPORT_W,
        VIEWPORT_H,
        "Tem's Chess",
        nullptr,
        nullptr
    );
    GLFWimage icon{};

    int channels;

    icon.pixels = stbi_load(
        "resources/icons/chess_icon.png",
        &icon.width,
        &icon.height,
        &channels,
        4
    );

    if (icon.pixels)
    {
        glfwSetWindowIcon(
            window,
            1,
            &icon
        );

        stbi_image_free(icon.pixels);
    }

    if (window == nullptr)
    {
        throw ErrorObj{
            WINDOW_FAILED_INIT,
            "GLFW failed to create the window"
        };
    }

    return window;
}

void GuiManager::guiWindowCleanUp(
    GLFWwindow* window)
{
    if (window != nullptr)
    {
        glfwDestroyWindow(window);
    }

    glfwTerminate();
}

void GuiManager::framebufferSizeCallback(
    GLFWwindow* window,
    int width,
    int height)
{
    (void)window;

    const int viewportSize =
        std::min(width, height);

    const int viewportX =
        (width - viewportSize) / 2;

    const int viewportY =
        (height - viewportSize) / 2;

    glViewport(
        viewportX,
        viewportY,
        viewportSize,
        viewportSize
    );
}

int GuiManager::mouseToSquare(
    GLFWwindow* window)
{
    double cursorX = 0.0;
    double cursorY = 0.0;

    glfwGetCursorPos(
        window,
        &cursorX,
        &cursorY
    );

    int windowWidth = 0;
    int windowHeight = 0;

    glfwGetWindowSize(
        window,
        &windowWidth,
        &windowHeight
    );

    int framebufferWidth = 0;
    int framebufferHeight = 0;

    glfwGetFramebufferSize(
        window,
        &framebufferWidth,
        &framebufferHeight
    );

    if (
        windowWidth <= 0 ||
        windowHeight <= 0 ||
        framebufferWidth <= 0 ||
        framebufferHeight <= 0
        )
    {
        return -1;
    }

    const double framebufferCursorX =
        cursorX *
        static_cast<double>(framebufferWidth) /
        static_cast<double>(windowWidth);

    const double framebufferCursorY =
        cursorY *
        static_cast<double>(framebufferHeight) /
        static_cast<double>(windowHeight);

    const float viewportSize =
        static_cast<float>(
            std::min(
                framebufferWidth,
                framebufferHeight
            )
            );

    const float viewportX =
        (framebufferWidth - viewportSize) / 2.0f;

    const float viewportY =
        (framebufferHeight - viewportSize) / 2.0f;

    const float boardSize =
        viewportSize * 0.8f;

    const float boardLeft =
        viewportX + viewportSize * 0.1f;

    const float boardTop =
        viewportY + viewportSize * 0.1f;

    const float squarePixelSize =
        boardSize / 8.0f;

    if (
        framebufferCursorX < boardLeft ||
        framebufferCursorX >= boardLeft + boardSize ||
        framebufferCursorY < boardTop ||
        framebufferCursorY >= boardTop + boardSize
        )
    {
        return -1;
    }

    const int column = static_cast<int>(
        (framebufferCursorX - boardLeft) /
        squarePixelSize
        );

    const int rowFromTop = static_cast<int>(
        (framebufferCursorY - boardTop) /
        squarePixelSize
        );

    const int row = 7 - rowFromTop;

    return row * 8 + column;
}

// -----------------------------------------------------------------------------
// Drawing
// -----------------------------------------------------------------------------

void GuiManager::drawBoard()
{
    boardShader.Activate();
    BoardVAO.Bind();

    glDrawElements(
        GL_TRIANGLES,
        boardIndexCount,
        GL_UNSIGNED_INT,
        nullptr
    );
}

void GuiManager::drawPieces()
{
    pieceShader.Activate();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(
        GL_TEXTURE_2D,
        atlasTexture
    );

    PieceVAO.Bind();

    glDrawElements(
        GL_TRIANGLES,
        pieceIndexCount,
        GL_UNSIGNED_INT,
        nullptr
    );
}

// -----------------------------------------------------------------------------
// Resource cleanup
// -----------------------------------------------------------------------------

void GuiManager::cleanUpOpenGLResources()
{
    if (atlasTexture != 0)
    {
        glDeleteTextures(
            1,
            &atlasTexture
        );

        atlasTexture = 0;
    }

    BoardVAO.Delete();
    BoardVBO.Delete();
    BoardEBO.Delete();
    boardShader.Delete();

    PieceVAO.Delete();
    PieceVBO.Delete();
    PieceEBO.Delete();
    pieceShader.Delete();

    boardIndexCount = 0;
    pieceIndexCount = 0;
}

// -----------------------------------------------------------------------------
// Main loop
// -----------------------------------------------------------------------------

int GuiManager::guiMainLoop()
{
    GLFWwindow* window = nullptr;

    try
    {
        window = guiWindowSetUp();
    }
    catch (const ErrorObj& error)
    {
        guiWindowCleanUp(window);
        std::cerr << error.error_msg << '\n';
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize GLAD.\n";
        guiWindowCleanUp(window);
        return -1;
    }

    glfwSetFramebufferSizeCallback(
        window,
        &GuiManager::framebufferSizeCallback
    );

    int framebufferWidth = 0;
    int framebufferHeight = 0;

    glfwGetFramebufferSize(
        window,
        &framebufferWidth,
        &framebufferHeight
    );

    framebufferSizeCallback(
        window,
        framebufferWidth,
        framebufferHeight
    );

    ClassicChess game;

    try
    {
        initializeShaders();
        initializeTexture();
        initializeBoard();
        initializePieces(game);
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "GUI initialization failed: "
            << error.what()
            << '\n';

        cleanUpOpenGLResources();
        guiWindowCleanUp(window);

        return -1;
    }

    std::cout
        << "Tem's Chess started.\n"
        << "Human: White\n"
        << "AI: Black\n"
        << "AI depth: "
        << aiDepth
        << "\n\n";

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        processMouseInput(
            window,
            game
        );

        processAITurn(game);

        glClearColor(
            0.82f,
            0.76f,
            0.71f,
            1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);

        drawBoard();
        drawPieces();

        glfwSwapBuffers(window);
    }

    cleanUpOpenGLResources();
    guiWindowCleanUp(window);

    return 0;
}
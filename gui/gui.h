#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../game.h"
#include "ebo.h"
#include "shader.h"
#include "vao.h"
#include "vbo.h"

#include <cstddef>
#include <vector>

// The current chess atlas contains six piece columns and two color rows.
constexpr int ATLAS_COLUMNS = 6;
constexpr int ATLAS_ROWS = 2;

constexpr float CELL_WIDTH = 1.0f / ATLAS_COLUMNS;
constexpr float CELL_HEIGHT = 1.0f / ATLAS_ROWS;

class GuiManager
{
public:
    GuiManager() = default;

    int guiMainLoop();

private:
    // -------------------------------------------------------------------------
    // Window setup
    // -------------------------------------------------------------------------

    static void framebufferSizeCallback(
        GLFWwindow* window,
        int width,
        int height
    );

    GLFWwindow* guiWindowSetUp();
    void guiWindowCleanUp(GLFWwindow* window);

    int VIEWPORT_W = 800;
    int VIEWPORT_H = 800;

    enum ERROR
    {
        UNKNOWN,
        WINDOW_FAILED_INIT,
        GLFW_FAILED_INIT
    };

    struct ErrorObj
    {
        ERROR error = UNKNOWN;
        const char* error_msg = "";
    };

    // -------------------------------------------------------------------------
    // Vertex data
    // -------------------------------------------------------------------------

    struct AtlasCell
    {
        int column;
        int row;
    };

    struct UVRegion
    {
        float uMin;
        float vMin;
        float uMax;
        float vMax;
    };

    struct textured_vertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z = 0.0f;

        GLfloat u;
        GLfloat v;
    };

    struct colored_vertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z = 0.0f;

        GLfloat r;
        GLfloat g;
        GLfloat b;
    };

    struct Color
    {
        GLfloat r;
        GLfloat g;
        GLfloat b;
    };

    // -------------------------------------------------------------------------
    // CPU-side mesh generation
    // -------------------------------------------------------------------------

    std::vector<colored_vertex> getVerticesForBoard();
    std::vector<GLuint> getBoardIndices();

    std::vector<textured_vertex> getVerticesForPieces(
        ClassicChess& game
    );

    std::vector<GLuint> getPieceIndices(
        std::size_t pieceCount
    );

    AtlasCell getAtlasCell(
        ClassicChess::PieceTypeBit piece
    );

    UVRegion getUVRegion(
        int column,
        int row
    );

    // -------------------------------------------------------------------------
    // OpenGL initialization and drawing
    // -------------------------------------------------------------------------

    void initializeBoard();
    void initializePieces(ClassicChess& game);
    void initializeShaders();
    void initializeTexture();

    void drawBoard();
    void drawPieces();

    void cleanUpOpenGLResources();

    // -------------------------------------------------------------------------
    // Game interaction
    // -------------------------------------------------------------------------

    enum class GameMode
    {
        PLAYER_VS_PLAYER,
        PLAYER_VS_AI
    };

    GameMode gameMode = GameMode::PLAYER_VS_AI;

    // Human plays White by default.
    bool aiPlaysWhite = false;
    int aiDepth = 2;

    int selectedSquare = -1;
    bool leftMouseWasPressed = false;

    bool aiMovePending = true;
    bool waitOneFrameBeforeAI = false;
    bool gameFinished = false;

    int mouseToSquare(GLFWwindow* window);

    void processMouseInput(
        GLFWwindow* window,
        ClassicChess& game
    );

    void processAITurn(
        ClassicChess& game
    );

    bool isAITurn(
        const ClassicChess& game
    ) const;

    bool selectedPieceBelongsToCurrentPlayer(
        const ClassicChess& game,
        int square
    ) const;

    bool checkAndLogGameOver(
        ClassicChess& game
    );

    void rebuildPieces(
        ClassicChess& game
    );

    // -------------------------------------------------------------------------
    // OpenGL resources
    // -------------------------------------------------------------------------

    VAO BoardVAO;
    VBO BoardVBO;
    EBO BoardEBO;
    Shader boardShader;

    VAO PieceVAO;
    VBO PieceVBO;
    EBO PieceEBO;
    Shader pieceShader;

    GLuint atlasTexture = 0;

    GLsizei boardIndexCount = 0;
    GLsizei pieceIndexCount = 0;
};
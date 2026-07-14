#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../game.h"
#include <algorithm>
#include <array>
#include <vector>

constexpr int ATLAS_COLUMNS = 6;
constexpr int ATLAS_ROWS = 5;

constexpr float CELL_WIDTH = 1.0f / ATLAS_COLUMNS;
constexpr float CELL_HEIGHT = 1.0f / ATLAS_ROWS;



class GuiManager {
	


	public:
		
		// vertex pipeline
		struct textured_vertex {

			GLfloat x;
			GLfloat y;
			GLfloat z = 0.0f;

			GLfloat u;
			GLfloat v;
		};

		struct colored_vertex {

			GLfloat x;
			GLfloat y;
			GLfloat z = 0.0f;

			GLfloat r;
			GLfloat g;
			GLfloat b;
		};

		struct Color {
			GLfloat r;
			GLfloat g;
			GLfloat b;
		};

		struct pieceText {

			std::array<textured_vertex, 4> vertices;

		};

		struct squareText {

			std::array<colored_vertex, 4> vertices;

		};



		std::vector<colored_vertex> getVerticesForBoard();
		std::vector<GLuint> getBoardIndices();
		
		std::vector<pieceText> getVerticesForPieces(ClassicChess& game);


		int VIEWPORT_W = 800;
		int VIEWPORT_H = 800;

		enum ERROR {
			UNKNOWN,
			WINDOW_FAILED_INIT,
			GLFW_FAILED_INIT
		};

		struct ErrorObj {
			ERROR error = UNKNOWN;
			const char* error_msg;
		};

		GuiManager() {

		};

		static void framebufferSizeCallback(GLFWwindow* window,
			int width,
			int height);

		int guiMainLoop();
		GLFWwindow* guiWindowSetUp();
		void guiWindowCleanUp(GLFWwindow* window);
		int mouseToSquare(GLFWwindow* window);


};

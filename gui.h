#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GuiManager {

	public:

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

		int guiMainLoop();
		GLFWwindow* guiWindowSetUp();
		void guiWindowCleanUp(GLFWwindow* window);

};

#pragma once

#include <glad/glad.h>
#include <string>

class Shader
{
public:
	enum ERROR {
		UNKNOWN,
		FILE_READ_FAILED
	};

	struct ErrorObj {
		ERROR error = UNKNOWN;
		const char* error_msg;
	};

    GLuint ID;

    Shader(const char* vertexFile, const char* fragmentFile);

    void Activate();
    void Delete();

private:

    static std::string readFile(const char* filepath);

};
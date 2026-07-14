#pragma once

#include <glad/glad.h>
#include <string>

class Shader
{
public:
    enum ERROR
    {
        UNKNOWN,
        FILE_READ_FAILED
    };

    struct ErrorObj
    {
        ERROR error = UNKNOWN;
        const char* error_msg;
    };

    GLuint ID = 0;

    Shader() = default;

    Shader(
        const char* vertexFile,
        const char* fragmentFile
    );

    ~Shader()
    {
        if (ID != 0)
        {
            glDeleteProgram(ID);
        }
    }

    // A shader program cannot have two owners.
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Move constructor
    Shader(Shader&& other) noexcept
        : ID(other.ID)
    {
        // The old object no longer owns the program.
        other.ID = 0;
    }

    // Move assignment
    Shader& operator=(Shader&& other) noexcept
    {
        if (this != &other)
        {
            // Delete anything currently owned by this object.
            if (ID != 0)
            {
                glDeleteProgram(ID);
            }

            // Take ownership from other.
            ID = other.ID;

            // Prevent other's destructor from deleting it.
            other.ID = 0;
        }

        return *this;
    }

    void Activate() const
    {
        glUseProgram(ID);
    }

    void Delete()
    {
        if (ID != 0)
        {
            glDeleteProgram(ID);
            ID = 0;
        }
    }

private:
    static std::string readFile(const char* filepath);
};
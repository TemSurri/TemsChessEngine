#pragma once
#include <glad/glad.h>
#include <utility>

class VBO
{
public:
    GLuint ID = 0;

    VBO() = default;

    VBO(const void* data, GLsizeiptr size)
    {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(
            GL_ARRAY_BUFFER,
            size,
            data,
            GL_STATIC_DRAW
        );
    }

    ~VBO()
    {
        if (ID != 0)
        {
            glDeleteBuffers(1, &ID);
        }
    }

    // No copying
    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    // Move constructor
    VBO(VBO&& other) noexcept
        : ID(other.ID)
    {
        other.ID = 0;
    }

    // Move assignment
    VBO& operator=(VBO&& other) noexcept
    {
        if (this != &other)
        {
            // Delete anything this object already owns.
            if (ID != 0)
            {
                glDeleteBuffers(1, &ID);
            }

            ID = other.ID;
            other.ID = 0;
        }

        return *this;
    }

    void Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, ID);
    }

    void Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Delete()
    {
        if (ID != 0)
        {
            glDeleteBuffers(1, &ID);
            ID = 0;
        }
    }
};
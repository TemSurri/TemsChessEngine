#pragma once
#include <glad/glad.h>

class EBO
{
public:
    GLuint ID = 0;

    EBO() = default;

    EBO(const GLuint* indices, GLsizeiptr size)
    {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            size,
            indices,
            GL_STATIC_DRAW
        );
    }

    ~EBO()
    {
        if (ID != 0)
        {
            glDeleteBuffers(1, &ID);
        }
    }

    EBO(const EBO&) = delete;
    EBO& operator=(const EBO&) = delete;

    EBO(EBO&& other) noexcept
        : ID(other.ID)
    {
        other.ID = 0;
    }

    EBO& operator=(EBO&& other) noexcept
    {
        if (this != &other)
        {
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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    }

    void Delete()
    {
        if (ID != 0)
        {
            glDeleteBuffers(1, &ID);
            ID = 0;
        }
    }

    void Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
};
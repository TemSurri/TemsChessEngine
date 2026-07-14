#pragma once

#include <glad/glad.h>
#include "vbo.h"

class VAO
{
public:
    GLuint ID = 0;

    VAO() = default;

    ~VAO()
    {
        if (ID != 0)
        {
            glDeleteVertexArrays(1, &ID);
        }
    }

    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

    VAO(VAO&& other) noexcept
        : ID(other.ID)
    {
        other.ID = 0;
    }

    VAO& operator=(VAO&& other) noexcept
    {
        if (this != &other)
        {
            if (ID != 0)
            {
                glDeleteVertexArrays(1, &ID);
            }

            ID = other.ID;
            other.ID = 0;
        }

        return *this;
    }

    void Create()
    {
        if (ID == 0)
        {
            glGenVertexArrays(1, &ID);
        }
    }

    void LinkAttrib(
        const VBO& vbo,
        GLuint layout,
        GLuint numComponents,
        GLenum type,
        GLsizei stride,
        const void* offset
    )
    {
        Bind();
        vbo.Bind();

        glVertexAttribPointer(
            layout,
            numComponents,
            type,
            GL_FALSE,
            stride,
            offset
        );

        glEnableVertexAttribArray(layout);
    }

    void Bind() const
    {
        glBindVertexArray(ID);
    }

    void Unbind() const
    {
        glBindVertexArray(0);
    }

    void Delete()
    {
        if (ID != 0)
        {
            glDeleteVertexArrays(1, &ID);
            ID = 0;
        }
    }
};
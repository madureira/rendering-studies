#pragma once

#include <cstring>

#include <GL/glew.h>

#include "Types.h"

inline void checkOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        const char* error;
        switch (err)
        {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }

        const char* relativePath = strstr(fname, "src/");
        if (!relativePath)
        {
            // If "src/" not found, use full path
            relativePath = fname;
        }

        LOG_ERROR("OpenGL: {} at {}:{}  {}", error, relativePath, line, stmt);
    }
}

#define GL(stmt)                                     \
    do                                               \
    {                                                \
        stmt;                                        \
        checkOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)

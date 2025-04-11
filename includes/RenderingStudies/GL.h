#pragma once

#include <cstring>
#include <type_traits>

#include <glad/glad.h>

#include "Types.h"

/**
 * @brief Converts OpenGL error codes to human-readable strings.
 */
inline const char* getGLErrorString(GLenum error)
{
    switch (error)
    {
        case GL_NO_ERROR:
            return "NO_ERROR";
        case GL_INVALID_ENUM:
            return "INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "INVALID_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "INVALID_FRAMEBUFFER_OPERATION";
        default:
            return "UNKNOWN_ERROR";
    }
}

/**
 * @brief Checks for OpenGL errors after a function call.
 */
template <typename T>
inline typename std::enable_if<!std::is_void<T>::value, T>::type
GL_CALL(T result, const char* stmt, const char* file, int32 line)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        const char* relativePath = strstr(file, "src/");
        if (!relativePath)
        {
            // If "src/" not found, use full path
            relativePath = file;
        }

        LOG_ERROR("OpenGL: {} at {}:{}  {}", getGLErrorString(err), relativePath, line, stmt);
    }
    return result;
}

/**
 * @brief Checks for OpenGL errors after a function call (void return type).
 */
inline void GL_CALL_VOID(const char* stmt, const char* file, int32 line)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        const char* relativePath = strstr(file, "src/");
        if (!relativePath)
        {
            // If "src/" not found, use full path
            relativePath = file;
        }

        LOG_ERROR("OpenGL: {} at {}:{}  {}", getGLErrorString(err), relativePath, line, stmt);
    }
}

#ifdef DEBUG
// Single macro for all OpenGL function calls
#define GL(stmt)                                 \
    do                                           \
    {                                            \
        stmt;                                    \
        GL_CALL_VOID(#stmt, __FILE__, __LINE__); \
    } while (0)

// Macro for functions that return values
#define GLR(stmt) GL_CALL(stmt, #stmt, __FILE__, __LINE__)
#else
// Single macro for all OpenGL function calls
#define GL(stmt) stmt

// Macro for functions that return values
#define GLR(stmt) stmt
#endif

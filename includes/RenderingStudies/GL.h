#pragma once

#include "Log.h"
#include "Types.h"
#include <cstring>
#ifdef __EMSCRIPTEN__
#  include <GLES3/gl3.h>
#  include <emscripten/emscripten.h>
#else
#  include <glad/glad.h>
#endif
#include <type_traits>

/**
 * @brief Converts OpenGL error codes to human-readable symbolic names.
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
 * @brief Returns a short hint for each OpenGL error to help debugging (e.g. shader link failure, invalid uniform).
 */
inline const char* getGLErrorHint(GLenum error)
{
    switch (error)
    {
        case GL_INVALID_ENUM:
            return "Check that all enum arguments are valid for this function.";
        case GL_INVALID_VALUE:
            return "Check that numeric arguments (e.g. size, count) are valid.";
        case GL_INVALID_OPERATION:
            return "Often: program not linked/failed to link; no program bound; invalid uniform location (-1); "
                   "or operation not allowed in current state (e.g. glDraw* without bound VAO).";
        case GL_OUT_OF_MEMORY:
            return "Driver ran out of memory. Try reducing texture size, mesh count, or resolution.";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "Framebuffer is incomplete or not bound where required.";
        default:
            return "See OpenGL docs for this error code.";
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

        LOG_ERROR("OpenGL {} (0x{:X}) in '{}'\n  at {}:{}\n  Hint: {}", getGLErrorString(err), (unsigned)err, stmt, relativePath, line, getGLErrorHint(err));
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

        LOG_ERROR("OpenGL {} (0x{:X}) in '{}'\n  at {}:{}\n  Hint: {}", getGLErrorString(err), (unsigned)err, stmt, relativePath, line, getGLErrorHint(err));
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

//
// Created by Krisu on 2020/2/29.
//

#include "Debug.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#ifdef ENGINE_DEBUG

template <typename T>
void DEBUG_LOG(T first) {
    std::clog << first << std::endl;
}

template <typename T, typename...Args>
void DEBUG_LOG(T first, Args...args) {
    std::clog << first << " ";
    if constexpr (sizeof...(args) > 0) {
        DEBUG_LOG(args...);
    }
}

void DEBUG_TEXTURE2D(const Texture &t) {
    static Shader debugShader {"shader/debug-texture.vert",
                               "shader/debug-texture.frag"};
    debugShader.UseShaderProgram();
    debugShader.Set("map", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(static_cast<int>(t.Type()), t.ID());
    SimpleMesh::renderQuad();
}

#else

template <typename T, typename...Args>
void DEBUG_LOG(T first, Args...args) { /* empty */ }

void DEBUG_TEXTURE2D(const Texture &t) { }

#endif

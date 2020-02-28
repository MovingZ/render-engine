#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 localPos;

void main() {
    localPos = aPos;

    mat4 rotView = mat4(mat3(view)); // remove translation
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);

    // xyww ensure that skybox z-buffer depth always max
    gl_Position = clipPos.xyww;
}
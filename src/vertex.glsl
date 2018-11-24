#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoords;
out vec2 tex_coords;
void main() {
    gl_Position = vec4(inPos, 1.0);
    tex_coords = vec2(inTexCoords.x / 160.0, inTexCoords.y / 144.0);
}
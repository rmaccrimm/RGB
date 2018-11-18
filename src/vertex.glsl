#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoords;
out vec2 texCoords;
uniform int scrollx;
uniform int scrolly;
uniform bool background;
void main() {
    gl_Position = vec4(inPos, 1.0);
    if (background == true) {
        // Top of screen at pixel 144, top of background at 256 so first shift up by 112
        texCoords = vec2(inTexCoords.x + scrollx, inTexCoords.y + 112.0 - scrolly) / 256.0;
    }
    else {
        texCoords = inTexCoords;
    }
}
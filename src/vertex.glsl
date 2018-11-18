#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoords;
out vec2 bg_tex_coords;
out vec2 sprite_tex_coords;
uniform int scrollx;
uniform int scrolly;
void main() {
    gl_Position = vec4(inPos, 1.0);
    // Top of screen at pixel 144, top of background at 256 so first shift up by 112
    bg_tex_coords = vec2(inTexCoords.x + scrollx, inTexCoords.y + 112.0 - scrolly) / 256.0;
    sprite_tex_coords = vec2(inTexCoords.x + 8, inTexCoords.y + 8) / 176;
}
#version 330 core
out vec4 frag_color;
in vec2 texCoords;
uniform usampler2D backround_texture;
uniform usampler2D sprite_texture;
uniform usampler2D sprite_priority;
uniform usampler2D window_texture;
void main() {
    uint val = texture(background_texture, texCoords).r;
    float col = float(val) / 255.0;
    frag_color = vec4(col, col, col, 1);
}
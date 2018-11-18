#version 330 core
out vec4 frag_color;
in vec2 texCoords;
uniform int tex_type; 
uniform usampler2D background_texture;
uniform usampler2D sprite_texture;
uniform usampler2D sprite_priority;
uniform usampler2D window_texture;
void main() {
    float col;
    if (tex_type == 0) {
        uint val = texture(background_texture, texCoords).r;
        col = float(val) / 255.0;
    }
    frag_color = vec4(col, col, col, 1);
}
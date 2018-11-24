#version 330 core
out vec4 frag_color;
in vec2 tex_coords;
uniform usampler2D screen_texture;
void main() {
    uint val = texture(screen_texture, tex_coords).r;
    float col = float(val) / 255.0;
    frag_color = vec4(col, col, col, 1);
}
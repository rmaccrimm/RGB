#version 330 core
out vec4 frag_color;
in vec2 bg_tex_coords;
in vec2 sprite_tex_coords;
uniform usampler2D background_texture;
uniform usampler2D sprite_texture;
uniform usampler2D sprite_priority;
uniform usampler2D window_texture;
void main() {
    float col;
    uint bg_val = texture(background_texture, bg_tex_coords).r;
    uint sprite_val = texture(sprite_texture, sprite_tex_coords).r;
    
    uint val;
    if (texture(sprite_texture, sprite_tex_coords).g != 0U) {
        val = sprite_val;
    }
    else {
        val = bg_val;
    }
    col = float(val) / 255.0;
    frag_color = vec4(col, col, col, 1);
}
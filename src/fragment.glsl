#version 430 core
out vec4 frag_color;
in vec2 texCoords;
uniform usampler2D screen_texture;
uniform usampler2DRect color_palette;
void main() {
    uint val = texture(screen_texture, texCoords).r;
    uint ucol = texelFetch(color_palette, ivec2(int(val), 0)).r;
    float col = float(ucol) / 255.0;
    frag_color = vec4(col, col, col, 1);
}
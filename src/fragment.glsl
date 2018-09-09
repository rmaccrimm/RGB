#version 430 core
out vec4 frag_color;
in vec2 texCoords;
uniform usampler2D screen_texture;
uniform usampler1D color_palette;
void main() {
    uint val = texture(screen_texture, texCoords).r;
    // float col = texture(color_palette, )
    float col = float(texelFetch(color_palette, int(val), 0).r) / 255.0;
    // float col = float(0xf0) / 255.0;
    // float col, col2;
    // if (val == uint(0)) {
        // col = 178.0 / 255.0;
        // col2 = col;
    // }
    // else if (val == uint(1)) {
        // col = 255.0 / 255.0;;
        // col2 = col;
    // }
    // else if (val == uint(2)) {
        // col = 102.0 / 255.0;
        // col2 = col;
    // }
    // else if (val == uint(3)) {
        // col = 0.0;
        // col2 = col;
    // }
    // else {
        // col = 200.0 / 255.0;
        // col2 = 100.0 / 255.0;
    // }
    frag_color = vec4(col, col, col, 1);
}
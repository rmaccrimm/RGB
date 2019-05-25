#version 330 core
out vec4 frag_color;
in vec2 tex_coords;
uniform usampler2D screen_texture;
uniform uint palette[4];

// Color palettes
// const uint super_gameboy[4] = uint[](0xf7e7c6U, 0xd68e49U, 0xa63725U, 0x331e50U);
// const uint grayscale[4] = uint[](0xffffffU, 0xb6b6b6U, 0x676767U, 0x000000U);
// const uint original_green[4] = uint[](0xe3eec0U, 0xaeba89U, 0x5e6745U, 0x202020U);
// const uint palette[4] = uint[](0xf7e7c6U, 0xd68e49U, 0xa63725U, 0x331e50U);

void main() {
    uint val = texture(screen_texture, tex_coords).r;
    vec3 rgb_col = vec3((palette[val] >> 16) & 0xffU, 
                        (palette[val] >> 8) & 0xffU, 
                        palette[val] & 0xffU) / 255.0;
    frag_color = vec4(rgb_col, 1);
}
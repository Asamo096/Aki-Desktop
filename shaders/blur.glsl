// 高斯模糊

uniform sampler2D tex;
uniform vec2 resolution;
uniform float radius;

vec4 blur(sampler2D tex, vec2 uv, vec2 resolution, float radius) {
    vec4 color = vec4(0.0);
    float total = 0.0;
    float kernel[9];
    
    // 3x3 高斯核
    kernel[0] = 1.0 / 16.0;
    kernel[1] = 2.0 / 16.0;
    kernel[2] = 1.0 / 16.0;
    kernel[3] = 2.0 / 16.0;
    kernel[4] = 4.0 / 16.0;
    kernel[5] = 2.0 / 16.0;
    kernel[6] = 1.0 / 16.0;
    kernel[7] = 2.0 / 16.0;
    kernel[8] = 1.0 / 16.0;
    
    vec2 offset = 1.0 / resolution * radius;
    
    int index = 0;
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            vec2 uv_offset = uv + vec2(x, y) * offset;
            color += texture(tex, uv_offset) * kernel[index];
            total += kernel[index];
            index++;
        }
    }
    
    return color / total;
}

// 圆角裁切

uniform vec2 size;
uniform float radius;

float rounded_rect(vec2 p, vec2 size, float radius) {
    vec2 q = abs(p) - size + radius;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

vec4 apply_rounded_corners(vec4 color, vec2 uv, vec2 size, float radius) {
    float d = rounded_rect(uv - size * 0.5, size * 0.5 - vec2(radius), radius);
    float alpha = 1.0 - smoothstep(-0.5, 0.5, d);
    return vec4(color.rgb, color.a * alpha);
}

// 工具函数

// 线性插值
float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

//  clamp 值到指定范围
float clamp(float x, float min_val, float max_val) {
    return min(max(x, min_val), max_val);
}

// 计算距离场
float distance_field(vec2 p, vec2 size, float radius) {
    vec2 q = abs(p) - size + radius;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

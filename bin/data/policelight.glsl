float circle(vec2 p, float radius, float e) {
    return smoothstep(radius - e, radius + e, length(p));
}

float circle(vec2 p, float radius) {
    return circle(p, radius, 0.01);
}

float horizontal(vec2 p, float height, float e) {
    return smoothstep(height - e, height + e, p.y);
}

float horizontal(vec2 p, float height) {
    return horizontal(p, height, 0.01);
}


void mainImage( out vec4 col, in vec2 p ) {
    p /= iResolution.xy;

    float texture = smoothstep(0.49, 0.51, p.y + p.x * tan(iGlobalTime + p.x * 500.0));

    float move = sin(iGlobalTime) * 0.05;

    float headMask = circle(p * vec2(1.2, 1.0) - vec2(0.5 + move, 0.2 + move), 0.75);

    float eyes = min(circle(p - vec2(0.25 + move, 0.65), 0.075), circle(p - vec2(0.75 + move, 0.65), 0.075));
    eyes = max(horizontal(p, 0.71, 0.0175), eyes);
    eyes = max(1.0 - horizontal(p, 0.68, 0.005), eyes);

    float mouth = circle(p - 0.5 + move * 0.25, 0.1, 0.005);
    mouth = max(horizontal(p, 0.45), mouth);

    float a = iGlobalTime * 25.0;
    p *= mat2(cos(a), -sin(a),
             sin(a), cos(a));

    col.rgb = vec3(p.x * 0.2, p.y * 0.5, p.x * p.y) * headMask * (1.0 - texture * 0.1) +
              vec3(p.x * 0.3, p.y * 0.2, p.y * 0.35) * texture * (1.0 - headMask) +
              vec3(1.0 - min(eyes, mouth)) * vec3(p.y * 0.75, p.y * 0.55, p.y * 0.75) * texture +
              length(p + 1.0) * 0.15;
}

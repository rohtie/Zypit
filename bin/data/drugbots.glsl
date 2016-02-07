void mainImage( out vec4 col, in vec2 p ) {
    p /= iResolution.xy;

    vec2 q = p;

    float r = length(p - 0.5);
    r = smoothstep(0.23, 0.25, r);
    r = max(r, smoothstep(0.49, 0.51, p.y + sin(p.x * iGlobalTime * 50.0)));
    r = max(r, smoothstep(0.39, 0.41, p.y));

    float a = p.x * 23.0 + iGlobalTime * 7.0;
    p *= mat2(cos(a), -sin(a),
              sin(a), cos(a));

    r = max(r, smoothstep(0.49, 0.51, p.y));

    col.rgb = vec3(0.1) - r + vec3(1.0 - p.y, p.x, p.y)
              + smoothstep(0.52, 0.51, length(q - vec2(0.5, 0.2))) * q.y * 1.5
              - smoothstep(0.05, 0.04, length(q - vec2(0.25, 0.52)))
              + smoothstep(0.045, 0.01, length(q - vec2(0.25, 0.52)))
              - smoothstep(0.05, 0.04, length(q - vec2(0.75, 0.52)))
              + smoothstep(0.045, 0.01, length(q - vec2(0.75, 0.52)));
}
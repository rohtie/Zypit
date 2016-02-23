void mainImage(out vec4 o, in vec2 p) {
    p = (p / iResolution.xy) - 0.5;
    p.x *= iResolution.x / iResolution.y;

    vec2 q = vec2(p.x, mod(p.y + iGlobalTime * 0.4, 0.15));
    q.x /= abs(q.y) * 8.0 + -1.0 + sin(iGlobalTime) * 0.6;

    float tri = smoothstep(0.2, 0.02, max(abs(q.y), smoothstep(0.6, 0.61, abs(q.x))));
    o.rgb = (1.0 - tri) * vec3((0.5 + p.y) * 0.75, (1.0 - abs(p.x)) * 0.02, (1.0 - p.y) * 0.2) +
            tri * vec3(1.0, p.y * 4.0, 0.0);
}

void mainImage( out vec4 col, in vec2 p) {
    p /= iResolution.xy;
    p = 2.0 * p - 1.0;
    p.x *= iResolution.x / iResolution.y;
    p *= iGlobalTime * 50.0;

    float result = max(abs(p.x), abs(p.y));

    float t =  1.0 + abs(sin(iGlobalTime)) * 5.0;

    result = mod(result, t);

    result = smoothstep(1.0, 0.95, result);

    col.rgb = vec3(result);
}

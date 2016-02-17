void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    float t = iGlobalTime;
    vec2 p = fragCoord.xy / iResolution.xy;

    vec2 q = p - vec2(0.422, 0.521);

    float r = smoothstep(
        0.58 - cos(atan(q.x, q.y) + texture2D(iChannel0, vec2(0.0, 0.0)).x * 0.4) * 0.55,
        0.0,
        length(q)
    );

    vec3 col = vec3(exp(p.y) * 0.32, 0.31, 0.27) +
               vec3(r) * vec3(exp(p.y), 0.47, 0.2);

    fragColor = vec4(col, 1.0);
}
float waves(vec2 p) {
    p.y += sin(p.x * 6.0) * 0.1;
    p.y -= sin(iGlobalTime) * 0.15;
    p.y += sin(p.x * 12.0) * 0.06 * sin(iGlobalTime);

    p.y = mod(p.y, 0.25 + sin(iGlobalTime)*0.05);

    float res = abs(p.y);
    res = smoothstep(0.11, 0.1, res);

    return res;
}

float circle(vec2 p) {
    p.y += sin(p.x * 7.0 + iGlobalTime) * 0.35;
    return smoothstep(0.6, 0.61, length(p));
}

void mainImage( out vec4 o, in vec2 p ) {
    p /= iResolution.xy;
    p -= 0.5;
    p.x *= iResolution.x / iResolution.y;


    float res = waves(p);
    res = max(res, circle(p));

    o.rgb = vec3(res) + (1.0 - circle(p)) * -abs(p.y) * 0.5 - circle(p) * 0.5 + circle(p) * (1.0 - length(p)) * 0.3;

    p.y += sin(p.x * 0.21 + texture2D(iChannel0, vec2((p.x + 1.0) * 0.5, 0.0)).r * 0.1);

    o.r /= p.y + 0.07 * 2.0;
    o.b += p.y * 0.4;

}

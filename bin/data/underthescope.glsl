float hash(float n) {
    return fract(sin(n) * 43758.5453);
}

void mainImage( out vec4 o, in vec2 p ) {
    p /= iResolution.xy;
    p = p * 2.0 - 1.0;
    p.x *= iResolution.x / iResolution.y;

    float time = iGlobalTime + 10.0;

    vec3 col = vec3(hash(time) * hash(p.x * p.y * time)) * 2.7;

    float a = 1.26;
    p *= mat2(-sin(a), cos(a),
              cos(a), sin(a));

    col.r += p.y + length(p) * 0.25;
    col.g += p.y * 0.7 + length(p) * 0.12;

    for (float i = 0.0; i < 100.0; i += 2.0) {
        vec2 circleLocation = vec2(hash(i), hash(i + 1.0)) - 0.5;

        circleLocation.x *= 3.0;
        circleLocation.y *= 2.0;

        circleLocation.x += 0.3 * sin(time * hash(i * 5.0));
        circleLocation.y += 0.2 * sin(time * hash(i * 6.0));

        float vibrate = abs(sin(time * hash(i) * 5.0)) * 0.01;
        float size = hash(i * 100.0) * 0.02 + 0.06;


        float result = smoothstep(size + 0.01 + vibrate, size + vibrate, length(p - circleLocation));

        col += result * 0.5;
    }

    col *= smoothstep(1.5, 0.85, length(p));
    col.b += 0.25;

    o.rgb = col;

}

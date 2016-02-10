// http://iquilezles.org/www/articles/smin/smin.htm
float smin (float a, float b, float k) {
    a = pow(a, k); b = pow(b, k);
    return pow((a*b)/(a + b), 1.0/k);
}

void mainImage (out vec4 col, in vec2 p) {
    float t = iGlobalTime;

    p /= iResolution.xy;

    vec2 q1 = p - vec2(0.13, 0.12);
    vec2 q2 = p - vec2(0.65 - sin(t/5.0)*0.25, 0.6 - cos(t)*0.05);
    vec2 q3 = p - vec2(0.5, 0.12);

    float r1 = length(q1) + 1.0 + cos(atan(q1.y, q1.x) * 6.0 + t*6.0);
    float r2 = length(q2);
    float r3 = smoothstep(0.0, 0.0, -q3.y);

    float r = smin(r1, r2, 1.0);
    r = max(r, r3);

    float eyes = (
        smoothstep(0.16, 0.01, length(q2 + vec2(0.1, -0.1)) + 0.08) +
        smoothstep(0.16, 0.01, length(q2 + vec2(0.28, -0.1)) + 0.089)
    );
    eyes = min(eyes, smoothstep(0.0, 0.001, q2.y - 0.11));
    r = max(r, eyes);

    float mouth = smoothstep(0.3, 0.28, length(q2 + vec2(0.22, 0.0)) + 0.26);
    mouth = min(mouth, smoothstep(0.0, 0.001, -q2.y));
    r = max(r, mouth);

    r = smoothstep(0.3, 0.27, r);

    col.rgb = (
        // Background
        vec3(0.125 + p.y*0.21) +
        vec3(0.13, p.x*p.y, sqrt(p.x*p.y*p.x)) +
        vec3(0.0, -p.x, -sqrt(p.y)) * vec3(0.25) +

        // Objects
        vec3(r) /
        vec3(smoothstep(0.27, 0.4, r2) + 0.26 + p.x*p.y) *
        vec3(p.y*0.51, p.x*p.y, 0.15) +

        // Specular
        vec3(smoothstep(0.6, 0.1, length(q2 - vec2(0.15, 0.07)) + 0.25) * p.x) *
        vec3(1.00, 0.12, 0.21)
    );
}

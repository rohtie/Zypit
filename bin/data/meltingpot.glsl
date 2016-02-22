float smin(float a, float b, float k) {
    float h = clamp( 0.5 + 0.5 * (b-a) / k, 0.0, 1.0 );
    return mix( b, a, h ) - k * h * (1.0 - h);
}

float clearify(float dist) {
    return smoothstep(0.19, 0.21, dist);
}

void mainImage( out vec4 col, in vec2 p ) {
    p /= iResolution.xy;
    p.x *= iResolution.x / iResolution.y;

    // Hard box vs Round box
    #if 1
    float r = length(max(abs(p - 0.4) - 0.1, 0.0));
    float g = length(max(abs(p - 0.6) - 0.1, 0.0));
    #else
    float r = max(abs(p - 0.4).x, abs(p - 0.4).y);
    float g = max(abs(p - 0.6).x, abs(p - 0.6).y);
    #endif

    r *= texture2D(iChannel0, vec2((p.x) * 0.5, 0.75)).r * 2.5;
    g *= texture2D(iChannel0, vec2((p.x) * 0.5, 0.75)).r * 0.25;

    // Create pattern
    #if 1
    float t = (iGlobalTime + 3000.0) / 150.0;
    p += t * 5.0;

    r += cos(t / 2.0 * p.x) * tan(t * p.y);
    g += sin(t * 3.0 * p.x) * cos(t / 5.0 * p.y);
    #endif

    vec2 c = vec2(0.0, 1.0) * 0.25;

    col.rgb = vec3(1.0) +
        clearify(min(r, g))       * c.xyy -
        clearify(mix(r, g, 0.5))  * c.yyx -
        clearify(smin(r, g, 0.2)) * c.yyy -
        clearify(max(r, g))       * c.xyy -
        clearify(max(-r, g))      * c.xyy;
}

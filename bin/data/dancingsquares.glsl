vec2 rotate(vec2 point, float angle) {
    return mat2(
        cos(angle), -sin(angle),
        sin(angle), cos(angle)
    )*point;
}

vec2 repeat(vec2 point, vec2 center) {
    return mod(point, center) - 0.5*center;
}

void mainImage( out vec4 col, in vec2 p ) {
    p /= iResolution.xy;

    vec2 center = vec2(1.0) / 5.0;

    center = repeat(p, center);

    float angle = 3.14/4.0 + texture2D(iChannel0, vec2(0.9, 0.75)).x * 3.14;
    center = rotate(center, angle);

    float square = abs(center.x) + abs(center.y);
    square = smoothstep(0.12, 0.11, square);

    vec2 eyecenter = rotate(center, 3.14/4.0);
    float eyes = min(length(eyecenter + vec2(-0.04, 0.0)), length(eyecenter + vec2(0.04, 0.0)));
    eyes = max(eyes, smoothstep(0.0, 0.12, eyecenter.y));
    eyes = smoothstep(0.02, 0.01, eyes);


    col.rgb = (
        // Background
        vec3(0.24 / sqrt(p.y + 0.2), 0.23, 0.11) +

        // Square
        vec3(square) *
        vec3(0.42 / p.y*0.52, 0.15 / p.y*0.82 + p.x*0.22, 0.21) +
        texture2D(iChannel1, center).rgb * 0.25 +

        // Eyes
        -vec3(eyes) +

        // Overlay
        vec3(-length(p - vec2(0.323))) * vec3(2.52, 1.23, 2.04) * 0.164 +

        // Specular
        vec3(square + cos(atan(center.x, center.y) * 0.25 - angle))*p.x*p.y
    );
}

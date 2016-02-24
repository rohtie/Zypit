vec2 solve(vec2 p, float upperLimbLength, float lowerLimbLength) {
    vec2 q = p * (0.5 + 0.5 * (upperLimbLength * upperLimbLength - lowerLimbLength * lowerLimbLength) / dot(p, p));

    float s = upperLimbLength * upperLimbLength / dot(q, q) - 1.0;

    if (s < 0.0) {
        return vec2(-100.0);
    }

    return q + q.yx * vec2(-1.0, 1.0) * sqrt(s);
}

float smin( float a, float b, float k ) {
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

float line(vec2 a, vec2 b, vec2 p) {
    vec2 pa = p - a;
    vec2 ba = b - a;

    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);

    return length(pa - ba * h) - 0.04;
}

float limb(vec2 p, vec2 target, float upperLimbLength, float lowerLimbLength) {
    vec2 joint = solve(target, upperLimbLength, lowerLimbLength);
    return min(line(vec2(0.0), joint, p), line(joint, target, p));
}

float limb(vec2 p, vec2 target) {
    return limb(p, target, 0.5, 0.5);
}

float legs(vec2 p) {
    float speed = 4.0;

    p.y += sin(iGlobalTime * speed) * 0.05;

    float leftLeg = limb(p,
        vec2(-0.1 + sin(iGlobalTime * speed) * 0.4,
             -0.7 + cos(iGlobalTime * speed) * 0.25)
    );

    float rightLeg = limb(p,
        vec2(-0.1 + sin(2.75 + iGlobalTime * speed) * 0.4,
             -0.7 + cos(2.75 + iGlobalTime * speed) * 0.25)
    );

    return smin(
        min(leftLeg, rightLeg),
        smin(length(p) - 0.2,
            line(vec2(0.0), vec2(0.3, 0.7), p) - p.y * 0.2,
            0.2
        ),
        0.3
    );
}

float arms(vec2 p) {
    p.y = 1.0 - p.y;
    p.y -= 0.25;
    p.x -= 0.3;

    float speed = 4.0;
    p.y += sin(iGlobalTime * speed) * 0.025;

    vec2 target = vec2(0.0, 0.5);
    vec2 ellipse = vec2(-0.5, 0.2);
    vec2 limbSize = vec2(0.5, 0.4);

    float leftLeg = limb(p,
        vec2(target.x - sin(iGlobalTime * speed) * ellipse.x,
             target.y - cos(iGlobalTime * speed) * ellipse.y),
        limbSize.x,
        limbSize.y
    );

    float rightLeg = limb(p,
        vec2(target.x - sin(2.75 + iGlobalTime * speed) * ellipse.x,
             target.y - cos(2.75 + iGlobalTime * speed) * ellipse.y),
        limbSize.x,
        limbSize.y
    );

    return smin(min(leftLeg, rightLeg), length(p) -0.15, 0.3);
}

float head(vec2 p) {
    float speed = 4.0;

    p.y += sin(iGlobalTime * speed) * 0.06;

    float head = length(p) - 0.3;

    float mouth = max(length(p - vec2(0.25, 0.0)) - 0.1, p.y);
    float eye = max(length(p - vec2(0.0, 0.06)) - 0.1, -p.y + 0.12);
    float hat = max(length(p - vec2(-0.35, 0.06)) - 0.1, -p.y + 0.12);

    return max(min(head, hat), -min(mouth, eye));
}

float tree(vec2 p, vec2 target) {
    vec2 q = p - target;

    q *= 2.0;
    q.x += mod(iGlobalTime * 10.0, 20.0);

    return mix(cos(atan(q.x, q.y) * 2.5), length(q), 0.8) - 0.2;
}

float grass(vec2 p) {
    p *= 0.5;
    p.y -= 0.5;

    p.x += iGlobalTime * 1.5;
    p.x = mod(p.x, 0.5);
    p.y += 0.7;


    float f = max(abs(p.x * 6.0), abs(p.y * 0.8));
    f = min(f, max(abs(p.x * 0.4), abs(p.y * 6.8)));
    f = min(f, max(abs(p.x * 0.4), abs((p.y - 0.25) * 6.8)));


    return f - 0.2;
}

void mainImage( out vec4 o, in vec2 p ) {
    p = (p / iResolution.xy) - 0.5;
    p.x *= iResolution.x / iResolution.y;

    p *= 3.0;
    p.y += 0.2;

    float f = min(legs(p), arms(p));
    f = smin(f, head(p - vec2(0.6, 1.1)), 0.1);
    f = smin(f, p.y + 0.9, 0.1);

    f = min(f, tree(p, vec2(4.0, -0.8)));

    p.y += sin(p.x * 0.2 + iGlobalTime);

    o.rgb = vec3(0.0) +
        smoothstep(0.00, 0.01, f) *
        vec3(1.6 * smoothstep(0.00, 0.01, grass(p)), p.y * 6.7, (p.y + 1.3) * 0.5);
}

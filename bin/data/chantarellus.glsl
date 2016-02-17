float smin (float a, float b, float k) {
    a = pow(a, k); b = pow(b, k);
    return pow((a*b)/(a + b), 1.0/k);
}

void mainImage( out vec4 col, in vec2 p ) {
    p /= iResolution.xy;


    p.x -= 0.13;

    float t = iGlobalTime;

    // Hat
    vec2 hat = p - vec2(0.47, 0.76);
    hat *= mat2(1.0, -0.27 + sin(t)*0.5,
                -0.1 - sin(t) * 0.21, 1.6);

    float hat_r = length(hat) * 1.2;
    hat_r += cos(atan(hat.x, hat.y)*10.0)* 0.0074;


    // Stalk
    vec2 stalk = p - vec2(0.5, 0.42);
    stalk *= mat2(3.4 + stalk.y * 5.0, 0.16 - sin(t)*0.02,
                  sin(t) * 0.025, 1.2);

    float stalk_r = length(stalk) * 1.25 + stalk.y * 0.57 + sin(t) * 0.05 + 0.052;

    // Shroom
    float shroom = min(hat_r + 0.034, stalk_r);
    shroom = smoothstep(0.15, 0.1 * pow(p.y + 0.2, 2.2), shroom - 0.15);

    float shroomBody = smin(hat_r, stalk_r, 0.87 - abs(sin(t))*0.05);
    shroomBody = smoothstep(0.20, 0.21, shroomBody);
    float shroomBody_m = 1.0 - shroomBody;

    // Hat details
    float hatDetails = smoothstep(0.4, 0.0, hat_r + length(
        hat*mat2(1.0, 0.0,
                 1.0, 0.0))
        * -cos(atan(hat.x * 1.4, hat.y * 1.2))
    ) * 0.85;

    // Stalk texture
    vec2 stalkTex = stalk * 2.4 * stalk_r;
    float angle = t;
    stalkTex *= mat2(1.0, sin(angle),
                     0.0, 1.0);

    float stalkT = smoothstep(2.1, 0.1, length(stalkTex));
    stalkT = pow(stalkT, 10.0);

    // Shadow
    vec2 shadow = p - vec2(0.5, 0.42 + sin(t)*0.05);
    shadow *= mat2(1.0 - shadow.y * 3.0, 0.15 + sin(t)*0.05,
                   0.0, 1.0 + sin(t)*0.15);

    float shadow_r = length(shadow);
    shadow_r = smoothstep(0.25, 0.06, shadow_r);

    // Composite
    col.rgb = (
        // Background
        sqrt(p.y) *
        vec3(0.12, 0.77, 0.2) *
        texture2D(iChannel1, sin(t)*0.031 + 1.0 - p * exp(p.y * 5.2)*0.25).rgb * 1.75 *
        shroomBody +

        (p * 1.55 - vec2(0.0, 0.55) + p.x*0.37).y * vec3(0.052, 0.12, 0.75) *
        shroomBody +

        // Textures
        texture2D(iChannel0, hat).rgb
        * smoothstep(0.44, 0.2, hat_r)
        * shroomBody_m +

        texture2D(iChannel0, p / stalkT).rgb
        * smoothstep(0.5, 0.0, stalk_r) * 1.5
        * shroomBody_m +

        // Lighting
        shroomBody * -shadow_r * 0.25 +
        shroom * 0.66 +
        -smoothstep(0.4, 0.0, stalk_r) * 0.45 +
        smoothstep(0.21, 0.1, stalk_r) * 0.32 +

        // Post
        (1.0 - hat_r) * 0.25 * shroomBody_m +
        shroomBody_m * pow(p.y, 2.0) * vec3(1.35, 1.25, 0.0) * 0.15 +
        -vec3(1.35, 1.25 + p.x, 0.0) * 0.06 +
        -hatDetails * shroomBody_m * 0.732 +
        + 0.0
    );
}

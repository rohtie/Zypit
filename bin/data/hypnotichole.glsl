void mainImage( out vec4 col, in vec2 p ) {
    p /= iResolution.xy;

    vec2 q = p - vec2(0.5);

    q /= 0.25 + iGlobalTime * 0.4;

    q *= mat2(sin(q.y)*10.0, sin(q.y + iGlobalTime)*2.0, 0.0, sin(atan(q.x, q.y*3.0))*5.0);
    q *= mat2(0.6, 1.0, 5.0, 1.2);

    col = vec4(smoothstep(0.0, 0.642, length(q)));
}

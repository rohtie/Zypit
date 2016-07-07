void main() {
    vec4 col = vec4(1.0);

    vec2 p = gl_FragCoord.xy;

    // OpenGL is designed around bottom up framebuffers and textures
    // Therefore we need to flip the pixels from the fbo vertically
    // to avoid that the saved image is upside down.
    p.y = iResolution.y - p.y;

    mainImage(col, p);
    outputColor = vec4(col.rgb, 1.0);
}

void main() {
    vec4 col = vec4(1.0);
    mainImage(col, gl_FragCoord.xy);
    outputColor = vec4(col.rgb, 1.0);
}

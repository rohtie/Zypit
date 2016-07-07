void mainImage(out vec4 col, in vec2 p) {
	p /= iResolution.xy;
	p -= 0.5;
	p.x *= iResolution.x / iResolution.y;

	p.x /= abs(0.5 - texture2D(iChannel0, p.yy + 0.5).r);
	
    col.rgb = vec3(p.y + 0.25, 0.0, 0.0) + smoothstep(0.3985, 0.4, length(p));
}

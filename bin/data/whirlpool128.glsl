vec2 map (vec3 point) {
    float t = iGlobalTime;
    float r = point.y;


    point.xz *= mat2(cos(t), -sin(t),
                     sin(t), cos(t));

    float whirl0 = texture2D(iChannel1, point.xy * point.yz).r;


    float whirl1 = texture2D(iChannel1, point.xz * point.yx).r;


    if (t > 10.0 && mod(t, 2.0) > -1.0 && mod(t, 2.0) < 1.0) {
        whirl0 = whirl1;
    }

    float result = min(whirl0, whirl1);

    float material = 0.0;

    if (result == whirl0) {
        material = 1.0;
    }

    vec2 res = vec2(r + mix(whirl0, whirl1, mod(t, 200.0)), material);

    res.x += texture2D(iChannel0, point.xz * point.yx).r;


    return res;
}

vec3 getNormal(vec3 point) {
    vec2 extraPolate = vec2(0.002, 0.0);

    return normalize(vec3(
        map(point + extraPolate.xyy).x,
        map(point + extraPolate.yxy).x,
        map(point + extraPolate.yyx).x
    ) - map(point).x);
}

vec3 light = normalize(vec3(2.0, 2.0, 10.0));

void mainImage (out vec4 color, in vec2 point) {
    point /= iResolution.xy;
    point = 2.0 * point - 1.0;
    point.x *= iResolution.x / iResolution.y;

    vec3 cameraPosition = vec3(0.0, 0.0, 2.0);

    vec3 rayDirection = normalize(vec3(point, -1.0));

    //float distance = intersect(cameraPosition, rayDirection);

    vec3 rayOrigin = cameraPosition;

    const float maxDistance = 10.0;
    const float distanceTreshold = 0.001;
    const int maxIterations = 50;

    float distance = 0.0;

    vec2 currentDistance = vec2(1.0, 0.0);
    float material = 0.0;

    for (int i = 0; i < maxIterations; i++) {
        if (currentDistance.x < distanceTreshold || distance > maxDistance) {
            break;
        }

        currentDistance = map(rayOrigin + rayDirection * distance);

        distance += currentDistance.x;
        material = currentDistance.y;
    }

    vec3 col = vec3(0.0);

    vec3 blue_d = vec3(0.05, 0.01, 0.35);
    vec3 blue_s = vec3(0.7, 1.0, 0.95);

    vec3 oran_d = vec3(0.45, 0.25, 0.15);
    vec3 oran_s = vec3(1.0, 0.5, 0.25);

    vec3 diff = blue_d;
    vec3 spec = blue_s;

    if (distance < maxDistance) {
        vec3 point = cameraPosition + rayDirection * distance;
        vec3 normal = getNormal(point);

        if (material == 1.0) {
            diff = mix(blue_d, oran_d, 0.25 + sin(iGlobalTime) * 0.25);
            spec = oran_s;
        }

        col += diff;
        col += spec * max(dot(normal, light), 0.0);

        vec3 halfVector = normalize(light + normal);
        col += vec3(1.0) * pow(max(dot(normal, halfVector), 0.0), 1024.0);

        col -= point.z * 0.25;
        col *= 2.0;

    }

    color.rgb = col;
}

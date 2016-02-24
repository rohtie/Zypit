float smin( float a, float b, float k ) {
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

vec3 triPlanar(sampler2D tex, vec3 normal, vec3 point) {
    vec3 cX = texture2D(tex, point.yz).rgb;
    vec3 cY = texture2D(tex, point.xz).rgb;
    vec3 cZ = texture2D(tex, point.xy).rgb;

    vec3 blend = abs(normal);
    blend /= blend.x + blend.y + blend.z + 0.001;

    return blend.x * cX + blend.y * cY + blend.z * cZ;
}

float map (vec3 point) {
    return smin(min(length(point - vec3(1.05, 0.5, 0.5)) - 1.0,
               length(point - vec3(-1.0, -0.5, -0.5)) - 1.0),
              point.y + 0.6, 0.6
              );
}

vec3 getNormal(vec3 point) {
    vec2 extraPolate = vec2(0.002, 0.0);

    return normalize(vec3(
        map(point + extraPolate.xyy),
        map(point + extraPolate.yxy),
        map(point + extraPolate.yyx)
    ) - map(point));
}

float intersect (vec3 rayOrigin, vec3 rayDirection) {
    const float maxDistance = 10.0;
    const float distanceTreshold = 0.001;
    const int maxIterations = 50;

    float distance = 0.0;

    float currentDistance = 1.0;

    for (int i = 0; i < maxIterations; i++) {
        if (currentDistance < distanceTreshold || distance > maxDistance) {
            break;
        }

        vec3 point = rayOrigin + rayDirection * distance;
        currentDistance = map(point) +
                          triPlanar(iChannel0, getNormal(point), point).r *
                          (-0.1 + -abs(texture2D(iChannel1, point.xz * point.yx * point.zz).r));

        distance += currentDistance;
    }

    if (distance > maxDistance) {
        return -1.0;
    }

    return distance;
}

vec3 light = normalize(vec3(4.0, 6.0, 2.0));

void mainImage (out vec4 color, in vec2 point) {
    point /= iResolution.xy;
    point = 2.0 * point - 1.0;
    point.x *= iResolution.x / iResolution.y;

    vec3 cameraPosition = vec3(
        0.0 + cos(iGlobalTime),
        0.2 + sin(iGlobalTime) * 0.5 + 0.25,
        2.5 + cos(iGlobalTime * 1.5) * 0.5
    );
    vec3 rayDirection = normalize(vec3(point, -1.0));

    float distance = intersect(cameraPosition, rayDirection);

    vec3 col = vec3(0.0);

    if (distance > 0.0) {
        vec3 point = cameraPosition + rayDirection * distance;
        vec3 normal = getNormal(point);

        col += vec3(0.05, 0.01, 0.35);
        col += vec3(0.7, 1.0, 0.95) * max(dot(normal, light), 0.0);

        vec3 halfVector = normalize(light + normal);
        col += vec3(1.0) * pow(max(dot(normal, halfVector), 0.0), 1024.0);

        float att = clamp(1.0 - length(light - point) / 5.0, 0.0, 1.0); att *= att;
        col *= att;

        col *= vec3(smoothstep(0.25, 0.75, map(point + light))) + 0.5;

        if (point.y == 0.1) {
            col = vec3(50.0);
        }

        col *= triPlanar(iChannel0, normal, point);

        col *= 5.0;


    }

    color.rgb = col;
}

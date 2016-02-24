float smin( float a, float b, float k ) {
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

float capsule ( vec3 p, vec3 a, vec3 b, float r ) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h ) - r;
}

bool distanceCmp(float distanceA, float distanceB, float eps) {
    return distanceA > distanceB - eps && distanceA < distanceB + eps;
}

bool distanceCmp(float distanceA, float distanceB) {
    return distanceCmp(distanceA, distanceB, 0.05);
}

vec2 map (vec3 point) {
    float material = 0.0;

    float t = 0.1 + sin(iGlobalTime * 0.001) * 50000.0;
    float ti = 1.5 - sin(iGlobalTime) * 0.5;
    float j = 1.12;
    point.zx *= mat2(-sin(j), cos(j), cos(j), sin(j));

    point.zy *= mat2(-sin(ti), cos(ti), cos(ti), sin(ti));

    float body = capsule(point, vec3(0.0, 1.0, 0.0), vec3(0.0, -2.0, 0.0), 0.5);

    float mouth = smin(
        capsule(point, vec3(-0.3, 0.35, 0.4), vec3(-0.1, 0.17, 0.5), 0.09),
        capsule(point, vec3(0.3, 0.35, 0.4), vec3(0.1, 0.17, 0.5), 0.09),
        0.2);

    vec3 q = point;
    q.x += sin(q.y * 5.0) * 0.175;
    float bodyDeco = capsule(q, vec3(0.0, -0.5, 0.5), vec3(0.0, -2.0, 0.5), 0.11);


    point.x = abs(point.x);

    vec3 brow_point = point;
    brow_point.y -= sin(point.x * 5.0) * 0.175;
    float brows = capsule(brow_point, vec3(0.1, 0.8, 0.5), vec3(0.3, 0.7, 0.4), 0.05);


    float eyes = length(point - vec3(0.2, 0.6, 0.5)) - 0.1;

    float pupils = length(point - vec3(0.2, 0.6, 0.6)) - 0.075;

    float arms = smin(
        capsule(point, vec3(0.4, -0.3, 0.0), vec3(0.7, -0.75, 0.7), 0.21),
        capsule(point, vec3(0.5, -0.75, 1.3), vec3(0.7, -0.75, 0.9), 0.21),
        0.15);

    float fingers = min(capsule(point, vec3(0.6, -0.65, 1.1), vec3(0.2, -0.4, 1.1), 0.12),
                        min(capsule(point, vec3(0.6, -0.65, 1.1), vec3(0.2, -0.4, 1.8), 0.09),
                            min(capsule(point, vec3(0.4, -0.7, 1.5), vec3(0.4, -0.7, 1.8), 0.09),
                                capsule(point, vec3(0.4, -0.8, 1.4), vec3(0.4, -0.9, 1.7), 0.09))));

    float composite = smin(body, eyes, 0.1);
    composite = smin(composite, mouth, 0.07);
    composite = smin(composite, arms, 0.15);
    composite = min(composite, pupils);
    composite = smin(composite, bodyDeco, 0.06);
    composite = smin(composite, brows, 0.04);
    composite = smin(composite, fingers, 0.2);


    float distance = composite +
        texture2D(iChannel0, point.xx * point.yy * point.zz *
                             mat2(-sin(t), cos(t),
                                  cos(t), sin(t))).r * (0.05) ;

    if (distanceCmp(distance, body)) {
        material = 1.0;
    }
    else if (distanceCmp(distance, pupils, 0.02)) {
        material = 4.0;
    }
    else if (distanceCmp(distance, eyes, 0.1)) {
        material = 2.0;
    }
    else if (distanceCmp(distance, min(mouth, bodyDeco))) {
        material = 3.0;
    }

    return vec2(distance, material);
}

vec2 intersect (vec3 rayOrigin, vec3 rayDirection) {
    const float maxDistance = 10.0;
    const float distanceTreshold = 0.001;
    const int maxIterations = 50;

    float distance = 0.0;

    float currentDistance = 1.0;

    vec2 result;

    for (int i = 0; i < maxIterations; i++) {
        if (currentDistance < distanceTreshold || distance > maxDistance) {
            break;
        }

        result = map(rayOrigin + rayDirection * distance);
        currentDistance = result.x;

        distance += currentDistance;
    }

    if (distance > maxDistance) {
        return vec2(-1.0, 0.0);
    }

    return vec2(distance, result.y);
}

vec3 getNormal(vec3 point) {
    vec2 extraPolate = vec2(0.002, 0.0);

    return normalize(vec3(
        map(point + extraPolate.xyy).x,
        map(point + extraPolate.yxy).x,
        map(point + extraPolate.yyx).x
    ) - map(point).x);
}

vec3 light = normalize(vec3(5.0, 5.0, 5.0));

void mainImage (out vec4 color, in vec2 point) {
    point /= iResolution.xy;
    point = 2.0 * point - 1.0;
    point.x *= iResolution.x / iResolution.y;

    vec2 p = point;

    vec3 cameraPosition = vec3(-0.5, 0.0, 2.0);
    vec3 rayDirection = normalize(vec3(point, -1.0));

    vec2 result = intersect(cameraPosition, rayDirection);

    float distance = result.x;
    float material = result.y;

    vec3 col = vec3(0.0);

    if (result.x > 0.0) {
        vec3 point = cameraPosition + rayDirection * distance;
        vec3 normal = getNormal(point);

        if (material == 1.0) {
            col += vec3(0.5, 0.21, 0.2);
        }
        else if (material == 2.0) {
            col += vec3(0.5);
        }
        else if (material == 3.0) {
            col += vec3(0.6, 0.0, 0.0);
        }
        else if (material == 4.0) {}
        else {
            col += vec3(0.5, 0.21, 0.2);
        }

        col += vec3(0.7, 1.0, 0.95) * max(dot(normal, light), 0.0);

        vec3 halfVector = normalize(light + normal);
        col += vec3(1.0) * pow(max(dot(normal, halfVector), 0.0), 1024.0);

        float att = clamp(1.0 - length(light - point) / 5.0, 0.0, 1.0); att *= att;
        col *= att;

    }
    else {
        col = vec3(0.3 + p.x * 0.04, 0.05 + p.x * p.y * 0.2, smoothstep(0.2, 0.7 + sin(p.y * 52.0 * iGlobalTime), (length(p) - 1.0))) * 1.5;
    }

    col *= 1.5;

    color.rgb = col;
}

// Change to 1 if you are on windows to see a more interesting version
#define WINDOWS 0

// Enable if you want to see the casette from more angles
#define ROTATE 0

// Enable if you have a good graphics card
#define MULTISAMPLING 0

// Disable if you want to see a still frame
#define ANIMATE 1
#define FRAME 0.52


float Sphere( vec3 p, float s ) {
    return length(p)-s;
}

float Box( vec3 p, vec3 b ) {
    vec3 d = abs(p) - b;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float RoundBox( vec3 p, vec3 b, float r ) {
    return length(max(abs(p)-b,0.0))-r;
}



float Intersect ( float d1, float d2 ) {
    return max(-d2,d1);
}

float Union ( float d1, float d2 ) {
    return min(d1, d2);
}

vec3 Repeat ( vec3 p, vec3 c ) {
    return mod(p,c)-0.5*c;
}

vec3 rotate (vec3 point) {
    float a = iGlobalTime * 0.5;

    return point * mat3(1.0, 0.0, 0.0,
                        0.0, -sin(a), cos(a),
                        0.0, cos(a), sin(a));
}


float map (vec3 point) {
    #if ROTATE
    point = rotate(point);
    #endif

    #if WINDOWS
    point = Repeat(-point, -point);
    #endif

    float casette = (
        Union(
            Intersect(
                RoundBox(point, vec3(0.9, 0.5, 0.01), 0.075),
                Union(
                    Sphere(point + vec3(0.5, 0.1, 0.0), 0.17),
                    Sphere(point + vec3(-0.5, 0.1, 0.0), 0.17)
                )
            ),
            Box(point - vec3(0.0, 0.42, 0.0), vec3(0.8, 0.12, 0.1)) - point.y * 0.07
        )
    );

    #if ANIMATE
    float frame = sin(iGlobalTime);
    #else
    float frame = FRAME;
    #endif

    return mix(
        casette,
        Box(point, texture2D(iChannel0, point.xx * point.yy * point.zz).rgb),
        -clamp(frame, 0.0, 1.0)
    );
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

        currentDistance = map(rayOrigin + rayDirection * distance);

        distance += currentDistance;
    }

    if (distance > maxDistance) {
        return -1.0;
    }

    return distance;
}

vec3 getNormal(vec3 point) {
    vec2 extraPolate = vec2(0.002, 0.0);

    return normalize(vec3(
        map(point + extraPolate.xyy),
        map(point + extraPolate.yxy),
        map(point + extraPolate.yyx)
    ) - map(point));
}

vec3 light = normalize(vec3(0.0, 2.0, 3.0));

vec3 render(vec2 point) {
    #if WINDOWS
    vec3 cameraPosition = vec3(0.0, 0.0, 2.0);
    #else
    vec3 cameraPosition = vec3(0.0, 0.0, 1.0);
    #endif

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
    }
    else {
        return vec3(0.0, 0.05, 0.1);
    }

    return col;
}

void mainImage (out vec4 color, in vec2 point) {
    point /= iResolution.xy;
    point = 2.0 * point - 1.0;
    point.x *= iResolution.x / iResolution.y;

    #if MULTISAMPLING
    vec3 e = vec3(0.001, 0.0, -0.001);
    color.rgb = (
        render(point) +
        render(point - e.xy) +
        render(point + e.xy) +
        render(point + e.yx) +
        render(point - e.yx) +
        render(point - e.x) +
        render(point + e.x) +
        render(point + e.xz) +
        render(point + e.zx)
    ) / 9.0;
    #else
    color.rgb = render(point);
    #endif
}

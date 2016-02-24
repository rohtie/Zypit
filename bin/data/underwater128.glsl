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

float hash(float n) {
    return fract(sin(n) * 43758.5453);
}

float noise(in vec2 x) {
    vec2 p = floor(x);
    vec2 f = fract(x);

    f = f * f * (3.0 - 2.0 * f);

    float n = p.x + p.y * 57.0;

    float res = mix(mix(hash(n +  0.0), hash(n +  1.0), f.x),
                    mix(hash(n + 57.0), hash(n + 58.0), f.x), f.y);

    return res;
}

vec3 repeat( vec3 p, vec3 c ) {
    vec3 q = mod(p,c)-0.5*c;
    return q;
}

float caps( vec3 p, vec3 a, vec3 b, float r ) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h ) - r;
}

float cube( vec3 p, vec3 b ) {
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) +
         length(max(d,0.0));
}

vec2 map(vec3 point) {
    float material = 0.0;

    float result = point.y + noise(point.xz + iGlobalTime);
    result *= point.y + texture2D(iChannel0, point.xz * 0.05 + iGlobalTime * 0.1).r;

    return vec2(result, material);
}

vec3 getNormal(vec3 point) {
    vec2 extraPolate = vec2(0.002, 0.0);

    return normalize(vec3(
        map(point + extraPolate.xyy).x,
        map(point + extraPolate.yxy).x,
        map(point + extraPolate.yyx).x
    ) - map(point).x);
}

vec3 light = normalize(vec3(4.0, 6.0, 2.0));

void mainImage (out vec4 color, in vec2 point) {
    point /= iResolution.xy;
    point = 2.0 * point - 1.0;
    point.x *= iResolution.x / iResolution.y;

    /*vec3 cameraPosition = vec3(
        0.0 + cos(iGlobalTime),
        0.0 + sin(iGlobalTime) * 0.5 + 0.65,
        2.5 + cos(iGlobalTime * 1.5) * 0.5
    );

    float a = -2.5;
    cameraPosition.xy *= mat2(-sin(a), cos(a),
                              cos(a), sin(a));
    */

    vec3 cameraPosition = vec3(0.0, -1.0, 3.0);

    vec3 rayDirection = normalize(vec3(point, -1.0));

    const float maxDistance = 10.0;
    const float distanceTreshold = 0.001;
    const int maxIterations = 50;

    float distance = 0.0;

    float material = -1.0;

    float currentDistance = 1.0;

    for (int i = 0; i < maxIterations; i++) {
        if (currentDistance < distanceTreshold || distance > maxDistance) {
            break;
        }

        vec3 point = cameraPosition + rayDirection * distance;


        float person = point.y;

        vec2 mapResult = map(point);

        material = mapResult.y;

        currentDistance = mapResult.x -
                          triPlanar(iChannel0, getNormal(point), point).r * 0.15;

        distance += currentDistance;
    }

    if (distance > maxDistance) {
        distance = -1.0;
    }

    vec3 col = vec3(0.0);

    if (distance > 0.0) {
        vec3 point = cameraPosition + rayDirection * distance;
        vec3 normal = getNormal(point);

        if (material == 0.0) {
            col += vec3(0.05, 0.01, 0.35);
            col += vec3(0.7, 1.0, 0.95) * max(dot(normal, light), 0.0);

            vec3 halfVector = normalize(light + normal);
            col += vec3(1.0) * pow(max(dot(normal, halfVector), 0.0), 1024.0);

            float att = clamp(1.0 - length(light - point) / 5.0, 0.0, 1.0); att *= att;
            col *= att;

            col *= triPlanar(iChannel0, normal, point);
        }
        else {
            col += vec3(0.35, 0.31, 0.05);
            col += vec3(0.7, 1.0, 0.95) * max(dot(normal, light), 0.0);

            vec3 halfVector = normalize(light + normal);
            col += vec3(1.0) * pow(max(dot(normal, halfVector), 0.0), 1024.0);

            float att = clamp(1.0 - length(light - point) / 5.0, 0.0, 1.0); att *= att;
            col *= att;

            col *= triPlanar(iChannel1, normal, point);
        }

        col *= vec3(smoothstep(0.25, 0.75, map(point + light).x)) + 0.5;
        col *= 15.0;


    }
    else {
        point.y += sin(point.x * 0.05);
        col = vec3(0.0, -point.y * 0.75, -point.y);
    }

    color.rgb = col;
}

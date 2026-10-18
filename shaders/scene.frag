#version 330 core
in vec3 vPos;
in vec3 vNormal;
in vec3 vColor;
in float vMat;
out vec4 FragColor;

uniform vec3 uCamPos;
uniform vec3 uSunDir;     // direction TOWARD the sun
uniform vec3 uSunColor;
uniform vec3 uAmbient;
uniform vec3 uFogColor;
uniform float uFogK;
uniform float uNight;

float fogFactor() {
    float d = length(uCamPos - vPos);
    return exp(-pow(d * uFogK, 2.0));
}

void main() {
    // vMat: 0 = plain, 1 = building with procedural windows, 2 = emissive
    if (vMat > 1.5) {
        vec3 c = vColor * (0.85 + 0.6 * uNight);
        FragColor = vec4(mix(uFogColor, c, fogFactor()), 1.0);
        return;
    }
    vec3 N = normalize(vNormal);
    vec3 base = vColor;
    float spec = 0.06;
    float emit = 0.0;

    if (vMat > 0.5 && abs(N.y) < 0.5 && vPos.y > 3.2) {
        float u = (abs(N.x) > abs(N.z)) ? vPos.z : vPos.x;
        vec2 cell = vec2(floor(u / 3.2), floor(vPos.y / 3.6));
        vec2 f = vec2(fract(u / 3.2), fract(vPos.y / 3.6));
        if (f.x > 0.22 && f.x < 0.78 && f.y > 0.30 && f.y < 0.78) {
            float h = fract(sin(dot(cell, vec2(12.9898, 78.233))) * 43758.5453);
            emit = uNight * (h > 0.45 ? 1.0 : 0.0);
            base = mix(vec3(0.22, 0.34, 0.46), vec3(1.0, 0.85, 0.5), emit);
            spec = 0.7;
        }
    }

    float diff = max(dot(N, uSunDir), 0.0);
    vec3 V = normalize(uCamPos - vPos);
    vec3 H = normalize(uSunDir + V);
    float s = pow(max(dot(N, H), 0.0), 32.0) * spec;
    vec3 hemi = mix(uAmbient * 0.6, uAmbient, N.y * 0.5 + 0.5);
    vec3 col = base * (hemi + uSunColor * diff) + uSunColor * s;
    col = mix(col, base * 1.1, emit);
    FragColor = vec4(mix(uFogColor, col, fogFactor()), 1.0);
}

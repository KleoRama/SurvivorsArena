#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform float uTime;

void main()
{
    vec2 p = vUV - vec2(0.5);
    float dist = length(p);
    // atan(0, 0) al centro esatto dello schermo non e' definito
    float angle = (dist > 0.0001) ? atan(p.y, p.x) : 0.0;

    // gradiente con centro viola verso bordi quasi neri
    vec3 centerColor = vec3(0.08, 0.03, 0.13);
    vec3 edgeColor   = vec3(0.01, 0.008, 0.02);
    vec3 color = mix(centerColor, edgeColor, smoothstep(0.0, 0.78, dist));

    //effetto pavimento a lastre
    vec2 tileUV = vUV * vec2(11.0, 14.0);
    tileUV.x = tileUV.x + 0.5 * mod(floor(tileUV.y), 2.0);

    vec2 tileId = floor(tileUV);
    vec2 cell = fract(tileUV);

    // variazione di luminosita' per ogni lastra
    //valori scelti casualmente (essenzialmente un seed)
    float tileTone = fract(sin(dot(tileId, vec2(12.9898, 78.233))) * 43758.5453);
    
    // fughe scure lungo i bordi (0.025 su X e 0.05 su Y compensano il formato 2:1)
    float borderX = smoothstep(0.0, 0.025, min(cell.x, 1.0 - cell.x));
    float borderY = smoothstep(0.0, 0.05,  min(cell.y, 1.0 - cell.y));
    float grout = borderX * borderY;
    vec3 stoneColor = color * (0.85 + 0.45 * tileTone);
    color = mix(vec3(0.005, 0.004, 0.01), stoneColor, grout);

    // effetto spirale che ruota nel tempo
    float swirl = 0.5 + 0.5 * sin(angle * 6.0 + dist * 10.0 - uTime * 1.4);
    swirl = pow(swirl, 4.0);

    // bagliore tra viola e rosso scuro
    vec3 violetGlow = mix(
        vec3(0.32, 0.08, 0.52),
        vec3(0.24, 0.03, 0.16),
        0.5 + 0.5 * sin(uTime * 0.7)
    );

    // Attenua il vortice verso i bordi e al centro esatto
    float mask = smoothstep(0.75, 0.06, dist) * smoothstep(0.0, 0.05, dist);
    color = color + violetGlow * swirl * 0.30 * mask;

    FragColor = vec4(color, 1.0);
}

precision highp float;

vec4 pack(in float depth) {
    const vec3 bit_shift = vec3(65536.0, 256.0, 1.0);
    const vec3 bit_mask  = vec3(0.0, 1.0/256.0, 1.0/256.0);
    vec3 res = fract(depth * bit_shift);
    res -= res.xyz * bit_mask;
    return vec4(res.xyz, 1.0);
}

void main() {
    float depth = ((gl_FragCoord.z / gl_FragCoord.w) / 15000.0);
    gl_FragColor = pack(depth);
}



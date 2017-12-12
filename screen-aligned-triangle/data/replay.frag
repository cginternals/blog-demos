#version 420 core

uniform sampler2D fragmentIndex;
uniform float threshold;

in vec2 v_uv;

out vec4 out_color;

vec3 hsl2rgb(in vec3 hsl) // https://www.shadertoy.com/view/4sS3Dc
{
    vec3 rgb = clamp( abs(mod(hsl[0] + vec3(0.0, 4.0, 2.0), 6.0) - 3.0)- 1.0, 0.0, 1.0);
    return hsl[2] + hsl[1] * (rgb - 0.5) * (1.0 - abs(2.0 * hsl[2] - 1.0));
}

void main()
{
    float index = texture(fragmentIndex, v_uv).r;
    if(threshold < index)
        discard;

    ivec2 textureSize = textureSize(fragmentIndex, 0);
    float numIndices = float(textureSize.x * textureSize.y);

    vec4 unpacked = unpackUnorm4x8(uint(index));
    vec3 hsl = vec3(6.0 * index/numIndices, unpacked.r, unpacked.g);

    // highlight newest pixel and fade out highlighting over the last 250000 pixels
    float fence = smoothstep(0.0, 1.0, (threshold - index) / 200000.0);

    vec3 color = hsl2rgb(vec3(hsl.r                                   // hue - continuously, not influenced by fence highlighting
                            , mix(1.0, hsl.g, fence)                  // saturation - restarts every 256 pixels
                            , mix(1.0, 0.05 + hsl.b * 0.85, fence))); // luminance - restarts every 65536 pixels, scale luminance to avoid extreme contrasts
    
    out_color = vec4(color, 1.0);
}

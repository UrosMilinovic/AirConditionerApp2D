#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;
out vec2 chTex;

uniform vec2 uPos;     // centar objekta u "svetskim" koordinatama (y: -1..1, x: -aspect..aspect)
uniform vec2 uScale;   // sirina i visina objekta
uniform float uAspect; // sirina / visina ekrana, da krug ostane krug na svakom monitoru

void main()
{
    vec2 p = inPos * uScale + uPos;
    gl_Position = vec4(p.x / uAspect, p.y, 0.0, 1.0);
    chTex = inTex;
}

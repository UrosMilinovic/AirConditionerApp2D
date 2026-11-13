#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex0;
uniform vec4 uColor;      // boja (bez teksture) ili nijansa/providnost (sa teksturom)
uniform bool uUseTex;
uniform bool uCircle;     // ako je true, odbacuje sve izvan kruga upisanog u kvadrat
uniform vec2 uTexOffset;  // za atlas: pocetak izabrane celije u teksturi
uniform vec2 uTexScale;   // za atlas: velicina izabrane celije (1,1 = cela tekstura)

void main()
{
    if (uCircle && length(chTex - vec2(0.5)) > 0.5) discard;

    if (uUseTex) outCol = texture(uTex0, uTexOffset + chTex * uTexScale) * uColor;
    else outCol = uColor;
}

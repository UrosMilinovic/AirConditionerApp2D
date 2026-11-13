#include "Render.h"
#include "Config.h"
#include "Game.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include "Util.h"

// ---------- Renderer: jedan kvadrat (VAO) + uniforme ----------
static struct Renderer {
    unsigned int shader = 0, vao = 0, vbo = 0;
    int locPos, locScale, locAspect, locColor, locUseTex, locCircle, locTexOff, locTexScale;
} R;

static unsigned int texFire, texSnow, texCheck, texAtlas, texTag;

static void preprocessTexture(unsigned& texture, const char* filepath) {
    texture = loadImageToTexture(filepath);
    glBindTexture(GL_TEXTURE_2D, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    // CLAMP_TO_EDGE da se ivice ne "prelivaju" (vazno za atlas)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initRender() {
    R.shader = createShader("basic.vert", "basic.frag");
    glUseProgram(R.shader);
    R.locPos = glGetUniformLocation(R.shader, "uPos");
    R.locScale = glGetUniformLocation(R.shader, "uScale");
    R.locAspect = glGetUniformLocation(R.shader, "uAspect");
    R.locColor = glGetUniformLocation(R.shader, "uColor");
    R.locUseTex = glGetUniformLocation(R.shader, "uUseTex");
    R.locCircle = glGetUniformLocation(R.shader, "uCircle");
    R.locTexOff = glGetUniformLocation(R.shader, "uTexOffset");
    R.locTexScale = glGetUniformLocation(R.shader, "uTexScale");
    glUniform1i(glGetUniformLocation(R.shader, "uTex0"), 0);
    glUniform1f(R.locAspect, aspect);

    float vertices[] = {
        -0.5f,  0.5f,  0.0f, 1.0f, 
        -0.5f, -0.5f,  0.0f, 0.0f, 
         0.5f, -0.5f,  1.0f, 0.0f, 
         0.5f,  0.5f,  1.0f, 1.0f  
    };
    glGenVertexArrays(1, &R.vao);
    glGenBuffers(1, &R.vbo);
    glBindVertexArray(R.vao);
    glBindBuffer(GL_ARRAY_BUFFER, R.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    preprocessTexture(texFire, "res/icon_fire.png");
    preprocessTexture(texSnow, "res/icon_snow.png");
    preprocessTexture(texCheck, "res/icon_check.png");
    preprocessTexture(texAtlas, "res/atlas_digits.png");
    preprocessTexture(texTag, "res/student_tag.png");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.30f, 0.38f, 0.48f, 1.0f);
}

void cleanupRender() {
    glDeleteTextures(1, &texFire);
    glDeleteTextures(1, &texSnow);
    glDeleteTextures(1, &texCheck);
    glDeleteTextures(1, &texAtlas);
    glDeleteTextures(1, &texTag);
    glDeleteBuffers(1, &R.vbo);
    glDeleteVertexArrays(1, &R.vao);
    glDeleteProgram(R.shader);
}

static void setQuad(float cx, float cy, float w, float h, float r, float g, float b, float a,
    bool useTex, bool circle, float u0 = 0.0f, float us = 1.0f) {
    glUniform2f(R.locPos, cx, cy);
    glUniform2f(R.locScale, w, h);
    glUniform4f(R.locColor, r, g, b, a);
    glUniform1i(R.locUseTex, useTex);
    glUniform1i(R.locCircle, circle);
    glUniform2f(R.locTexOff, u0, 0.0f);
    glUniform2f(R.locTexScale, us, 1.0f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

static void drawRect(float cx, float cy, float w, float h, float r, float g, float b, float a = 1.0f) {
    setQuad(cx, cy, w, h, r, g, b, a, false, false);
}

static void drawEllipse(float cx, float cy, float w, float h, float r, float g, float b, float a = 1.0f) {
    setQuad(cx, cy, w, h, r, g, b, a, false, true);
}

static void drawCircle(float cx, float cy, float radius, float r, float g, float b, float a = 1.0f) {
    drawEllipse(cx, cy, radius * 2, radius * 2, r, g, b, a);
}

static void drawTexture(unsigned int tex, float cx, float cy, float w, float h, float a = 1.0f) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    setQuad(cx, cy, w, h, 1.0f, 1.0f, 1.0f, a, true, false);
}

static int glyphIndex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c == '-') return 10;
    if (c == '*') return 11;
    if (c == 'C') return 12;
    return -1;
}

static void drawText(const std::string& s, float cx, float cy, float h, float r, float g, float b) {
    float gw = h * GLYPH_RATIO;
    float advance = gw * 0.85f;
    float x = cx - advance * (s.size() - 1) / 2.0f;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texAtlas);
    for (char c : s) {
        int i = glyphIndex(c);
        if (i >= 0)
            setQuad(x, cy, gw, h, r, g, b, 1.0f, true, false, (float)i / ATLAS_CELLS, 1.0f / ATLAS_CELLS);
        x += advance;
    }
}

static void drawTemperature(int value, float cx, float cy, float h, float r, float g, float b) {
    drawText(std::to_string(value) + "*C", cx, cy, h, r, g, b);
}

static void drawBasin() {
    float wall = L.basinWall;
    float left = L.basinX - L.basinW / 2;
    float bottom = L.basinY - L.basinH / 2;
    float innerW = L.basinW - 2 * wall;
    float innerH = L.basinH - wall;
    float innerBottom = bottom + wall;

    drawRect(L.basinX, innerBottom + innerH / 2, innerW, innerH, 1.0f, 1.0f, 1.0f, 0.12f);

    float waterH = innerH * waterLevel();
    if (waterH > 0.0f)
        drawRect(L.basinX, innerBottom + waterH / 2, innerW, waterH, 0.55f, 0.80f, 1.0f, 0.85f);

    drawRect(left + wall / 2, innerBottom + innerH / 2, wall, innerH, 0.85f, 0.92f, 1.0f, 0.8f);
    drawRect(left + L.basinW - wall / 2, innerBottom + innerH / 2, wall, innerH, 0.85f, 0.92f, 1.0f, 0.8f);
    drawRect(L.basinX, bottom + wall / 2, L.basinW, wall, 0.85f, 0.92f, 1.0f, 0.8f);
}

static void drawScreens() {
    for (int i = 0; i < 3; i++) {
        if (G.acOn) drawRect(L.scrX[i], L.scrY, L.scrW, L.scrH, 0.04f, 0.09f, 0.06f);
        else        drawRect(L.scrX[i], L.scrY, L.scrW, L.scrH, 0.0f, 0.0f, 0.0f);
    }
    if (!G.acOn) return;

    float textH = 0.12f;
    drawTemperature(G.targetTemp, L.scrX[0], L.scrY, textH, 0.45f, 1.0f, 0.55f);  
    drawTemperature(G.currentTemp, L.scrX[1], L.scrY, textH, 0.6f, 0.9f, 1.0f);   

    unsigned int icon = (G.targetTemp > G.currentTemp) ? texFire
        : (G.targetTemp < G.currentTemp) ? texSnow : texCheck;
    drawTexture(icon, L.scrX[2], L.scrY, 0.17f, 0.17f);
}

static void drawAC() {
    float acBottom = L.acY - L.acH / 2;

    drawRect(L.acX, L.acY, L.acW, L.acH, 1.0f, 1.0f, 1.0f);
    drawRect(L.acX, acBottom + 0.01f, L.acW, 0.02f, 0.78f, 0.80f, 0.84f); 

    drawRect(L.slotX, L.slotY, L.slotW + 0.016f, L.slotH + 0.016f, 0.35f, 0.37f, 0.40f); 
    drawRect(L.slotX, L.slotY, L.slotW, L.slotH, 0.08f, 0.08f, 0.10f);                    
    float flapH = L.slotH * (1.0f - G.doorOpen);                                          
    if (flapH > 0.0f) {
        float slotTop = L.slotY + L.slotH / 2;
        drawRect(L.slotX, slotTop - flapH / 2, L.slotW, flapH, 0.62f, 0.64f, 0.68f);
    }

    // lampica
    drawCircle(L.lampX, L.lampY, L.lampR + 0.008f, 0.2f, 0.2f, 0.22f);
    if (G.acOn) drawCircle(L.lampX, L.lampY, L.lampR, 1.0f, 0.1f, 0.1f);
    else        drawCircle(L.lampX, L.lampY, L.lampR, 0.35f, 0.35f, 0.38f);

    drawScreens();
}

static void drawDrops() {
    for (const Drop& d : G.drops)
        drawEllipse(d.x, d.y, DROP_W, DROP_H, 0.55f, 0.80f, 1.0f, 0.95f);
}

void drawScene() {
    glUseProgram(R.shader);
    glBindVertexArray(R.vao);

    drawBasin();
    drawAC();
    drawDrops();

    // providni beli sloj preko celog ekrana kad je zeljena temperatura ispod nule
    if (G.targetTemp < 0) {
        float frostAlpha = 0.075f * (float)(-G.targetTemp);
        drawRect(0.0f, 0.0f, 2.0f * aspect, 2.0f, 1.0f, 1.0f, 1.0f, frostAlpha);
    }

    drawTexture(texTag, L.tagX, L.tagY, L.tagW, L.tagH, 0.75f);
}
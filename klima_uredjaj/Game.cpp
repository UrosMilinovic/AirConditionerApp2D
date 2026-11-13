#include "Game.h"
#include "Config.h"

#include <algorithm>
#include <cstdlib>

GameState G;
Layout L;
float aspect = 1.0f;

void buildLayout(float aspectRatio) {
    aspect = aspectRatio;

    L.acW = std::min(2.4f, 2.0f * aspect - 0.2f);
    L.acH = 0.56f;
    L.acX = 0.0f;
    L.acY = 1.0f - L.acH / 2;            
    float acBottom = L.acY - L.acH / 2;

    L.slotW = L.acW * 0.6f;
    L.slotH = 0.08f;
    L.slotX = 0.0f;
    L.slotY = acBottom + 0.08f;

    L.lampR = 0.05f;
    L.lampX = L.acX + L.acW / 2 - 0.12f; 
    L.lampY = acBottom + 0.12f;

    L.scrW = L.acW * 0.2f;
    L.scrH = 0.20f;
    L.scrY = L.acY + 0.06f;
    L.scrX[0] = -L.acW * 0.27f;
    L.scrX[1] = 0.0f;
    L.scrX[2] = L.acW * 0.27f;

    L.basinW = 1.2f;
    L.basinH = 0.55f;
    L.basinWall = 0.025f;
    L.basinX = 0.0f;
    L.basinY = -1.0f + 0.05f + L.basinH / 2;

    L.tagW = 0.64f;
    L.tagH = 0.16f;
    L.tagX = -aspect + L.tagW / 2 + 0.03f;
    L.tagY = -1.0f + L.tagH / 2 + 0.03f;
}

float waterLevel() {
    return (float)G.waterSteps / (float)WATER_STEPS;
}

float waterSurfaceY() {
    float innerBottom = L.basinY - L.basinH / 2 + L.basinWall;
    float innerH = L.basinH - L.basinWall;
    return innerBottom + innerH * waterLevel();
}

// Nova kap izlazi iz otvora klime, na nasumicnom x 
static void spawnDrop() {
    float innerHalf = (L.basinW - 2 * L.basinWall) / 2;
    float lo = std::max(L.basinX - innerHalf, L.slotX - L.slotW / 2) + DROP_W;
    float hi = std::min(L.basinX + innerHalf, L.slotX + L.slotW / 2) - DROP_W;

    float t = (float)std::rand() / (float)RAND_MAX;
    Drop d;
    d.x = lo + (hi - lo) * t;
    d.y = L.slotY - L.slotH / 2;   
    d.vy = 0.0f;
    G.drops.push_back(d);
}

void turnOff() {
    G.acOn = false;      
    G.tempTimer = 0.0f;
    G.dropTimer = 0.0f;
}

void turnOn() {
    if (G.waterSteps >= WATER_STEPS) return; 
    G.acOn = true;
    G.tempTimer = 0.0f;
    G.dropTimer = 0.0f;
}

void toggleAC() {
    if (G.acOn) turnOff();
    else turnOn();
}

void emptyBasin() {
    G.waterSteps = 0;
}

void changeTarget(int delta) {
    G.targetTemp = std::max(-10, std::min(40, G.targetTemp + delta));
}

bool hitLamp(float x, float y) {
    float dx = x - L.lampX, dy = y - L.lampY;
    float r = L.lampR + 0.008f; 
    return dx * dx + dy * dy <= r * r;
}

void update(float dt) {
    if (G.acOn) G.doorOpen = std::min(1.0f, G.doorOpen + DOOR_SPEED * dt);
    else        G.doorOpen = std::max(0.0f, G.doorOpen - DOOR_SPEED * dt);

    if (G.acOn && G.currentTemp != G.targetTemp) {
        G.tempTimer += dt;
        while (G.tempTimer >= TEMP_STEP_TIME && G.currentTemp != G.targetTemp) {
            G.currentTemp += (G.currentTemp < G.targetTemp) ? 1 : -1;
            G.tempTimer -= TEMP_STEP_TIME;
        }
    }
    else G.tempTimer = 0.0f;

    // Kapi se ispustaju samo dok klima radi i pregrada je potpuno otvorena
    if (G.acOn && G.doorOpen >= 1.0f) {
        G.dropTimer += dt;
        while (G.dropTimer >= DROP_INTERVAL) {
            G.dropTimer -= DROP_INTERVAL;
            spawnDrop();
        }
    }

    // Tek kad kap dodirne povrsinu vode, nivo u lavoru raste
    for (size_t i = 0; i < G.drops.size(); ) {
        Drop& d = G.drops[i];
        d.vy += DROP_GRAVITY * dt;
        d.y -= d.vy * dt;

        if (d.y - DROP_H / 2 <= waterSurfaceY()) {
            G.drops.erase(G.drops.begin() + i);
            if (G.waterSteps < WATER_STEPS) G.waterSteps++;
            if (G.waterSteps >= WATER_STEPS && G.acOn) turnOff(); 
        }
        else ++i;
    }
}
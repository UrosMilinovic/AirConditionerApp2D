#pragma once
#include <vector>

// Raspored elemenata
struct Layout {
    float acX, acY, acW, acH;                        // telo klime
    float slotX, slotY, slotW, slotH;                // otvor za vazduh (iza pregrade)
    float lampX, lampY, lampR;                       // lampica
    float scrX[3], scrY, scrW, scrH;                 // ekrani
    float basinX, basinY, basinW, basinH, basinWall; // lavor
    float tagX, tagY, tagW, tagH;                    // znacka sa imenom
};

// Kap vode u padu 
struct Drop {
    float x, y;   
    float vy;     
};

// Stanje programa
struct GameState {
    bool  acOn = false;       
    int   targetTemp = 24;    
    int   currentTemp = 30;   
    float doorOpen = 0.0f;    
    int   waterSteps = 0;     
    float tempTimer = 0.0f;   
    float dropTimer = 0.0f;  
    std::vector<Drop> drops;  
};

extern GameState G;
extern Layout L;
extern float aspect;          

void  buildLayout(float aspectRatio);

float waterLevel();           
float waterSurfaceY();        

void  turnOn();               
void  turnOff();
void  toggleAC();
void  emptyBasin();           
void  changeTarget(int delta);
bool  hitLamp(float x, float y); 
void  update(float dt);
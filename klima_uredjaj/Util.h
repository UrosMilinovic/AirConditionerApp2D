#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned loadImageToTexture(const char* filePath);
// hotspot = tacka na slici kursora koja "klikce" (za daljinski: laserska tackica)
GLFWcursor* loadImageToCursor(const char* filePath, int hotspotX, int hotspotY);
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#define PI_BEZIER glm::pi<float>()

extern std::vector<float> points;

std::string textFileRead_bezier(char* fn);
void display_bezier();
float B0(float u);
float B1(float u);
float B2(float u);
float B3(float u);
void createPointsVector();
void addControlCircle(float x, float y, float z);
void createControlPointsVector();
void refresh_bezier();
void refreshControlCircles();
void init_bezier();
void reshape_bezier(int w, int h);
void keyboard_bezier(unsigned char key, int x, int y);
void mouse_bezier(int button, int state, int x, int y);

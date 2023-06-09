#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include "spheremesh.h"

#define PI glm::pi<float>()

extern int i;
extern int ball_wind;

std::string textFileRead_ball(char* fn);
void printShaderInfoLog_ball(GLuint obj);
void printProgramInfoLog_ball(GLuint obj);
void display_ball();
void init_ball();
void reshape_ball(int w, int h);
void mouse_ball(int button, int state, int x, int y);
void postAllWindowsRedisplay();

#pragma once
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <utility>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string>


#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")

char* filetobuf(const char* file);
void ReadObj(const std::string objfilename, std::vector<glm::vec3>& vertex, std::vector<glm::vec3>& normalVertex, std::vector<glm::vec2>& vtVertex);

enum  AXIS
{
	X,
	Y,
	Z
};
const glm::vec3 AXIS_X = glm::vec3(1, 0, 0);
const glm::vec3 AXIS_Y = glm::vec3(0, 1, 0);
const glm::vec3 AXIS_Z = glm::vec3(0, 0, 1);
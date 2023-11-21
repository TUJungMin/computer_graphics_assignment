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
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
using namespace std;
using namespace glm;

enum planet {
	sun = 0,
	earth1,
	earth2,
	earth3,
	moon1,
	moon2,
	moon3
};
enum rotate_type {
	cir_earth1 = 0,
	cir_earth2,
	cir_earth3,
	cir_moon1,
	cir_moon2,
	cir_moon3
};

const glm::vec3 AXIS_X = glm::vec3(1, 0, 0);
const glm::vec3 AXIS_Y = glm::vec3(0, 1, 0);
const glm::vec3 AXIS_Z = glm::vec3(0, 0, 1);
void ReadObj(const std::string objfilename, std::vector<glm::vec3>& vertex, std::vector<glm::vec3>& normalVertex, std::vector<glm::vec2>& vtVertex)
{
	int lineCount = 0;
	std::string line;
	std::string check[6];
	int vertexNum = 0;
	int normalNum = 0;
	int cordiNum = 0;
	std::ifstream inFile(objfilename);
	std::vector<glm::vec4>face;
	std::vector<glm::vec4>Noramlface;
	std::vector<glm::vec4>vtface;
	while (std::getline(inFile, line)) {
		if (line[0] == 'v' && line[1] == ' ') {
			vertexNum++;
		}
		if (line[0] == 'v' && line[1] == 'n') {
			normalNum++;
		}
		if (line[0] == 'v' && line[1] == 't') {
			cordiNum++;
		}
		std::cout << line << std::endl;
	}
	glm::vec4* vertexData = new glm::vec4[vertexNum];
	glm::vec4* normalData = new glm::vec4[normalNum];
	glm::vec2* cordinaterData = new glm::vec2[cordiNum];

	inFile.clear();
	inFile.seekg(0, std::ios::beg);
	vertexNum = 0;
	normalNum = 0;
	cordiNum = 0;
	char head[2];
	int faceNum[3];
	int vnNum[3];
	int vtNum[3];
	std::string nt;
	char n;
	char s;
	while (inFile >> std::noskipws >> head[0]) { //빈칸을 안 씹겠다
		if (head[0] == 'v') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				inFile >> std::skipws >> vertexData[vertexNum].x >> vertexData[vertexNum].y >> vertexData[vertexNum].z;
				vertexNum++;
			}
			else if (head[1] == 'n') {
				inFile >> std::skipws >> normalData[normalNum].x >> normalData[normalNum].y >> normalData[normalNum].z;
				normalNum++;
			}
			else if (head[1] == 't') {
				float trash;
				inFile >> std::skipws >> cordinaterData[cordiNum].x >> cordinaterData[cordiNum].y >> trash;
				cordiNum++;
			}
			head[1] = '\0';
		}
		if (head[0] == 'f') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				inFile >> std::skipws >> faceNum[0];
				inFile >> std::skipws >> n;
				inFile >> std::skipws >> vtNum[0];
				inFile >> std::skipws >> s;
				inFile >> std::skipws >> vnNum[0];


				inFile >> std::skipws >> faceNum[1];
				inFile >> std::skipws >> n;
				inFile >> std::skipws >> vtNum[1];
				inFile >> std::skipws >> s;
				inFile >> std::skipws >> vnNum[1];


				inFile >> std::skipws >> faceNum[2];
				inFile >> std::skipws >> n;
				inFile >> std::skipws >> vtNum[2];
				inFile >> std::skipws >> s;
				inFile >> std::skipws >> vnNum[2];

				glm::vec4 temp = { faceNum[0], faceNum[1], faceNum[2], 1 };//faceNum[3]
				glm::vec4 vttemp = { vtNum[0], vtNum[1], vtNum[2], 1 }; //vtNum[3]
				glm::vec4 vntemp = { vnNum[0], vnNum[1], vnNum[2], 1 };//vnNum[3]
				face.push_back(temp);
				vtface.push_back(vttemp);
				Noramlface.push_back(vntemp);
			}
			head[1] = '\0';
		}
	}
	for (auto iter = face.begin(); iter < face.end(); iter++) {
		vertex.push_back(vertexData[(int)(iter->x) - 1]);
		vertex.push_back(vertexData[(int)(iter->y) - 1]);
		vertex.push_back(vertexData[(int)(iter->z) - 1]);                     //버텍스 좌표
	}
	for (auto iter = vtface.begin(); iter < vtface.end(); iter++) {
		vtVertex.push_back(cordinaterData[(int)(iter->x) - 1]);
		vtVertex.push_back(cordinaterData[(int)(iter->y) - 1]);
		vtVertex.push_back(cordinaterData[(int)(iter->z) - 1]);                //텍스쳐 좌표
	}

	for (auto iter = Noramlface.begin(); iter < Noramlface.end(); iter++) {
		normalVertex.push_back(normalData[(int)(iter->x) - 1]);
		normalVertex.push_back(normalData[(int)(iter->y) - 1]);
		normalVertex.push_back(normalData[(int)(iter->z) - 1]);               //노멀 좌표
	}
	delete[] vertexData;
	delete[] cordinaterData;
	delete[] normalData;
	inFile.close();
}
char* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}
float WIDTH = 1000;
float HEIGHT = 1000;
bool button_y = FALSE; bool button_p = FALSE;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid Timer(int value);
//--- 필요한 변수 선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
vec3 POS[6] = { { -1,0,0 }, { 1,0,0 },{ 0,-1,0 }, { 0,1,0 },{ 0,0,-1 }, { 0,0,1 } };

bool button_m = FALSE;
pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}

std::vector <glm::vec3> C_vertex;
GLvoid Timer(int value);
struct animation {
	bool button = FALSE;
	float angle = 0.0f;
	bool activate = FALSE;
};
struct Sector {
	vec3 color[3];
	GLuint m_vbo;
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	Sector(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		color[0].r = 1; color[0].g = 0; color[0].b = 0;
		color[1].r = 0; color[1].g = 1; color[1].b = 0;
		color[2].r = 0; color[2].g = 0; color[2].b = 1;
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(POS), POS, GL_STATIC_DRAW);
		//Line_T_matrix = glm::translate(Line_R_matrix, glm::vec3(0.0, 0.0, 0.0));

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));


		for (int i = 0; i < 3; ++i) {
			glUniform3f(colorUniformLoc, color[i].r, color[i].g, color[i].b);
			glDrawArrays(GL_LINES, 2 * i, 2);
		}
	}

};
struct SPHERE {
	GLUquadricObj* qobj;
	float radian = 0.5f;

	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale = 1.0f;
	float speed;
	vec3 color;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 parents_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	SPHERE(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		color.r = urd(dre);
		color.g = urd(dre);
		color.b = urd(dre);
		move_pos.x += center.first;
		move_pos.y += center.second;

	}
	void Draw() {
		qobj = gluNewQuadric();
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");

		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(gimbalAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[1]), AXIS_Y);
		//model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[0]), AXIS_X);

		model_matrix = glm::translate(model_matrix, (move_pos));

		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);

		model_matrix = glm::scale(model_matrix, glm::vec3(scale, scale, scale));
		model_matrix = parents_matrix * model_matrix;
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniform3f(colorUniformLoc, color.r, color.g, color.b);
		if (button_m) {
			gluQuadricDrawStyle(qobj, GLU_FILL);
		}
		else
		{
			gluQuadricDrawStyle(qobj, GLU_LINE);
		}
		gluSphere(qobj, radian, 50, 50);
	}
};
struct CIR {
	vec3 color;
	GLuint m_vbo;
	float radian = 0.5f;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale = 1.0f;
	float speed;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 parents_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	CIR(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		color.r = 1;
		color.g = 0;
		color.b = 0;
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, C_vertex.size() * sizeof(vec3), &C_vertex[0], GL_STATIC_DRAW);
		//Line_T_matrix = glm::translate(Line_R_matrix, glm::vec3(0.0, 0.0, 0.0));

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");
		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(revolutionAngle[2]), AXIS_Z);
		
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[1]), AXIS_Y);
		//model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[0]), AXIS_X);
		model_matrix = glm::translate(model_matrix, (move_pos));

		model_matrix = glm::rotate(model_matrix, glm::radians(gimbalAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);

		model_matrix = glm::scale(model_matrix, glm::vec3(scale, scale, scale));
		model_matrix = parents_matrix * model_matrix;
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniform3f(colorUniformLoc, color.r, color.g, color.b);
		for (int i = 0; i < C_vertex.size() / 2; ++i) {
			glDrawArrays(GL_LINE_LOOP, 2 * i, 2);
		}
	}
};
vector <Sector> Line;
vector<SPHERE> sphere;
vector<CIR> cir;
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정

	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WIDTH, HEIGHT); // 윈도우의 크기 지정
	for (float i = 0; i < 360; i++) {
		C_vertex.emplace_back(cos(radians(i)), 0, sin(radians(i)));
	}
	glutCreateWindow("OPENGL"); // 윈도우 생성
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized" << endl;
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();
	Line.emplace_back(WIDTH / 2, HEIGHT / 2);
	sphere.emplace_back(WIDTH / 2, HEIGHT / 2);

	for (int i = 0; i < 3; ++i) {
		sphere.emplace_back(WIDTH, HEIGHT / 2);	//지구와 궤적
		cir.emplace_back(WIDTH / 2, HEIGHT / 2);
	}
	for (int i = 0; i < 3; ++i) {
		sphere.emplace_back(WIDTH, HEIGHT / 2);	//달과 궤적
		cir.emplace_back(WIDTH, HEIGHT / 2);
	}
	/*지구공전궤도 변경*/
	cir[cir_earth2].revolutionAngle[2] = -45.0f;
	cir[cir_earth3].revolutionAngle[2] = 45.0f;
	/*지구 짐벌각 설정*/
	//sphere[earth2].gimbalAngle[2] = 45.0f;
	//sphere[earth3].gimbalAngle[2] = -45.0f;

	//*달 공전궤도 설정*/cir[cir_moon2].rotateAngle[2] = sphere[earth2].gimbalAngle[2]; cir[cir_moon3].rotateAngle[2] = sphere[earth3].gimbalAngle[2];

	sphere[earth2].revolutionAngle[2] = -45.0f;
	sphere[earth3].revolutionAngle[2] = 45.0f;

	//sphere[moon1].rotateAngle[2] = sphere[earth1].gimbalAngle[2];
	//sphere[moon2].rotateAngle[2] = sphere[earth2].gimbalAngle[2];
	//sphere[moon3].rotateAngle[2] = sphere[earth3].gimbalAngle[2];

	sphere[moon2].gimbalAngle[2] = 0.0f;
	sphere[moon3].gimbalAngle[2] = 0.0f;
	cir[cir_moon2].gimbalAngle[2] = 45.0f;
	cir[cir_moon3].gimbalAngle[2] = -45.0f;
	
	//cir[cir_earth1].scale = 0.5f; cir[cir_earth2].scale = 0.5f; cir[cir_earth3].scale = 0.5f;
	sphere[sun].scale = 0.7f; 

	sphere[earth1].scale = 0.3f; 
	sphere[earth2].scale = 0.3f;
	sphere[earth3].scale = 0.3f;

	sphere[moon1].scale = 0.15f; 
	sphere[moon2].scale = 0.15f; 
	sphere[moon3].scale = 0.15f;

	glutKeyboardFunc(KeyBoardFunc);
	glutTimerFunc(20, Timer, 0);
	glutTimerFunc(20, Timer, 1);
	glutTimerFunc(20, Timer, 2);
	glutTimerFunc(20, Timer, 3);
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMainLoop(); // 이벤트 처리 시작
}
vec3 camerapos{ 0.50f,0.7f,1.7f };
void drawScene()
{
	{
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgramID);

		glm::vec3 cameraPos = camerapos; //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		if (button_p) {
			projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
		}
		else {
			projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- 투영 공간 설정: fovy, aspect, near, far
			projection = glm::translate(projection, glm::vec3(0.0, 0.0, -2.0)); //--- 공간을 z축 이동
		}
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	}

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	cir[cir_earth1].parents_matrix = sphere[sun].model_matrix;
	cir[cir_earth2].parents_matrix = sphere[sun].model_matrix;
	cir[cir_earth3].parents_matrix = sphere[sun].model_matrix;
	sphere[earth1].parents_matrix = sphere[sun].model_matrix;
	sphere[earth2].parents_matrix = sphere[sun].model_matrix;
	sphere[earth3].parents_matrix = sphere[sun].model_matrix;
	cir[cir_moon1].parents_matrix = sphere[earth1].model_matrix;
	cir[cir_moon2].parents_matrix = sphere[earth2].model_matrix;
	cir[cir_moon3].parents_matrix = sphere[earth3].model_matrix;
	sphere[moon1].parents_matrix = cir[cir_moon1].model_matrix;
	sphere[moon2].parents_matrix = cir[cir_moon2].model_matrix;
	sphere[moon3].parents_matrix = cir[cir_moon3].model_matrix;




	Line[0].Draw();
	for (int i = 0; i < cir.size(); ++i) {
		cir[i].Draw();
	}
	for (int i = 0; i < sphere.size(); ++i) {
		sphere[i].Draw();
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glutSwapBuffers(); //화면에 출력
}
GLvoid Timer(int value)
{
	switch (value) {
	case 0:
		glutPostRedisplay();
		glutTimerFunc(17, Timer, 0);
		break;
	case 1:
		sphere[earth1].revolutionAngle[1] -= 2.0f;
		cir[cir_moon1].revolutionAngle[1] += 1.5f;
		sphere[moon1].revolutionAngle[1] -= 2.0f;
		glutTimerFunc(17, Timer, 1);
		break;
	case 2:
		sphere[earth2].revolutionAngle[1] -= 1.5f;
		cir[cir_moon2].revolutionAngle[1] += 1.5f;
		sphere[moon2].revolutionAngle[1] -= 1.5f;
		glutTimerFunc(17, Timer, 2);
		break;
	case 3:
		sphere[earth3].revolutionAngle[1] -= 1.0f;
		cir[cir_moon3].revolutionAngle[1] += 1.0f;
		sphere[moon3].revolutionAngle[1] -= 1.0f;

		glutTimerFunc(17, Timer, 3);
		break;
	}
}

GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'm':
		button_m = !button_m;
		break;
	case 'z':
		sphere[sun].rotateAngle[2] += 1.0f;
		break;
	case 'Z':
		sphere[sun].rotateAngle[2] -= 1.0f;
		break;
	case 'y':
		sphere[sun].rotateAngle[1] += 1.0f;
		break;
	case 'Y':
		sphere[sun].rotateAngle[1] -= 1.0f;
		break;
	case 'w':
		sphere[sun].move_pos.y += 0.1f;
		break;
	case 's':
		sphere[sun].move_pos.y -= 0.1f;
		break;
	case 'a':
		sphere[sun].move_pos.x -= 0.1f;
		break;
	case 'd':
		sphere[sun].move_pos.x += 0.1f;
		break;
	case '=':
		sphere[sun].move_pos.z += 0.1f;
		break;
	case '-':
		sphere[sun].move_pos.z -= 0.1f;
		break;
	case 'p':
		button_p = !button_p;
		break;
	}
	glutPostRedisplay();

}


GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
void make_vertexShaders() {
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex3.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {//result == 0이면 오류가 있다.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}
void make_fragmentShaders() {
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment2.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}
GLuint make_shaderProgram() {
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기 - 두 세이더 붙어야됨, vertex - fragment는 짝이 맞아야됨
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		cerr << "ERROR: shader program 연결 실패\n" << errorLog << endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}



#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
using namespace std;
using namespace glm;


float WIDTH = 1000;
float HEIGHT = 1000;
int g_rotate = 0;

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

bool button_m = FALSE; bool button_M = FALSE; bool button_f = FALSE; bool button_F = FALSE;
bool button_e = FALSE; bool button_E = FALSE; bool button_t = FALSE; bool button_T = FALSE;
bool button_y = FALSE; bool button_Y = FALSE; bool button_r = FALSE; bool button_R = FALSE;
pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}


std::vector <glm::vec3> D_vertex; std::vector <glm::vec3> D_normal; std::vector <glm::vec2> D_uv;

GLvoid Timer(int value);
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
struct Dice {
	GLuint m_vbo;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f };
	vec3 color;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 parents_matrix = glm::mat4(1.0f); //단위행렬로 초기화

	Dice(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		color.r = urd(dre);
		color.g = urd(dre);
		color.b = urd(dre);
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * D_vertex.size(), &D_vertex[0], GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");

		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(gimbalAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[1]), AXIS_Y);
		//model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[0]), AXIS_X);

		model_matrix = glm::translate(model_matrix, (move_pos));

		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);

		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
		model_matrix = parents_matrix * model_matrix;

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));

		glUniform3f(colorUniformLoc, color.r, color.g, color.b);		//아랫면
		for (int i = 0; i < 6; ++i) {
			glDrawArrays(GL_TRIANGLES, 6 * i, 6);
		}

	}


};
struct CAMERA {
	bool right_angle = FALSE;
	glm::vec3 camerapos; //단위행렬로 초기화
	glm::mat4 CR_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::vec3 upvector = { 0.0f,1.0f,0.0f };
	void Draw() {
		glm::vec3 cameraPos = { camerapos.x,camerapos.y,camerapos.z }; //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0, 0, 0); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(upvector); //--- 카메라 위쪽 방향
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		glm::mat4 projection = glm::mat4(1.0f);
		if(!right_angle)
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- 투영 공간 설정: fovy, aspect, near, far
		else
			projection = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -20.0f, 20.0f);
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -2.0)); //--- 공간을 z축 이동
		projection = projection * CR_matrix; //--- 공간을 z축 이동
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
};
vector <Sector> Line;
vector <Dice> Map;
CAMERA camera[3];
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정

	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WIDTH, HEIGHT); // 윈도우의 크기 지정

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
	ReadObj("cube.obj", D_vertex, D_normal, D_uv);
	{
		for (int i = 0; i < 3; ++i) {
			camera[i].camerapos.x = 0.0f; camera[i].camerapos.y = 0.5f; camera[i].camerapos.z = 3.0f;
		}
		Map.emplace_back(500, 500);		//밑바닥
		Map.emplace_back(500, 500);		//크레인 몸통
		Map.emplace_back(500, 500);		//크레인 머리
		Map.emplace_back(500, 500);		//크레인 팔1
		Map.emplace_back(500, 500);		//크레인 팔2
		Map.emplace_back(500, 500);		//크레인 팔3
		Map.emplace_back(500, 500);		//크레인 팔4

		Map[0].scale[0] = 4.0f; Map[0].scale[1] = 4.0f; Map[0].scale[2] = 4.0f;
		Map[0].move_pos.y = -0.5f * Map[0].scale[1];

		Map[1].scale[0] = 0.7f; Map[1].scale[1] = 0.35f; Map[1].scale[2] = 0.7f;
		Map[1].move_pos.y = 0.5f * Map[1].scale[1];

		Map[2].scale[0] = 0.5f; Map[2].scale[1] = 0.5f; Map[2].scale[2] = 0.5f;
		Map[2].move_pos.y = 0.5f;


		Map[3].scale[0] = 0.2f; Map[3].scale[1] = 0.8f; Map[3].scale[2] = 0.2f;
		Map[3].move_pos.y = 0.5f;
		Map[3].move_pos.x -= 0.3f;

		Map[4].scale[0] = 0.2f; Map[4].scale[1] = 0.8f; Map[4].scale[2] = 0.2f;
		Map[4].move_pos.y = 0.5f;
		Map[4].move_pos.x += 0.3f;

		Map[5].scale[0] = 0.1f; Map[5].scale[1] = 0.2f; Map[5].scale[2] = 1.0f;
		Map[5].move_pos.z = 0.7f;
		Map[5].move_pos.x -= 0.3f;

		Map[6].scale[0] = 0.1f; Map[6].scale[1] = 0.2f; Map[6].scale[2] = 1.0f;
		Map[6].move_pos.z = 0.7f;
		Map[6].move_pos.x += 0.3f;
		Map[5].color.r = Map[6].color.r;
		Map[5].color.g = Map[6].color.g;
		Map[5].color.b = Map[6].color.b;
	}
	glutTimerFunc(20, Timer, 0);
	glutKeyboardFunc(KeyBoardFunc);
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMainLoop(); // 이벤트 처리 시작
}

void drawScene()
{
	Map[2].parents_matrix = Map[1].model_matrix;
	Map[3].parents_matrix = Map[2].model_matrix;
	Map[4].parents_matrix = Map[2].model_matrix;
	Map[5].parents_matrix = Map[1].model_matrix;
	Map[6].parents_matrix = Map[1].model_matrix;
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glViewport(0, 0, 500, 500);
	camera[0].Draw();
	Line[0].Draw();
	for (int i = 0; i < Map.size(); ++i) {
		Map[i].Draw();
	}

	glViewport(600, 600, 200, 200);
	camera[1].right_angle = TRUE;
	camera[1].camerapos.x = 0; camera[1].camerapos.y = 1.0f; camera[1].camerapos.z = 0;
	camera[1].upvector.x = 0; camera[1].upvector.y = 0.0f; camera[1].upvector.z = -1.0f;
	camera[1].Draw();
	Line[0].Draw();
	for (int i = 0; i < Map.size(); ++i) {
		Map[i].Draw();
	}
	glViewport(600, 0, 200, 200);
	camera[2].right_angle = TRUE;
	camera[2].camerapos.x = 0; camera[2].camerapos.y = 0.0f; camera[2].camerapos.z = 1.0f;
	camera[2].upvector.x = 0; camera[2].upvector.y = 1.0f; camera[2].upvector.z = 0.0f;
	camera[2].Draw();
	Line[0].Draw();
	for (int i = 1; i < Map.size(); ++i) {
		Map[i].Draw();
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glutSwapBuffers(); //화면에 출력
}
GLvoid Timer(int value)
{
	switch (value)
	{
	case 0:
		glutPostRedisplay();
		glutTimerFunc(20, Timer, 0);
		break;
	case 1:
		if (button_m)
		{
			Map[2].rotateAngle[1] += 2.0f;
			glutTimerFunc(20, Timer, 1);
		}
		else if (button_M) {
			Map[2].rotateAngle[1] -= 2.0f;
			glutTimerFunc(20, Timer, 1);
		}

		break;
	case 2:
		if (button_f)
		{
			Map[5].revolutionAngle[1] -= 2.0f;
			Map[6].revolutionAngle[1] += 2.0f;
			if (Map[5].revolutionAngle[1] <= -360.0f) {
				Map[5].revolutionAngle[1] = 0.0f;
			}
			if (Map[6].revolutionAngle[1] >= 360.0f) {
				Map[6].revolutionAngle[1] = 0.0f;
			}
			glutTimerFunc(20, Timer, 2);
		}
		else if (button_F) {
			Map[5].revolutionAngle[1] += 2.0f;
			Map[6].revolutionAngle[1] -= 2.0f;
			if (Map[5].revolutionAngle[1] >= 360.0f) {
				Map[5].revolutionAngle[1] = 0.0f;
			}
			if (Map[6].revolutionAngle[1] <= -360.0f) {
				Map[6].revolutionAngle[1] = 0.0f;
			}
			glutTimerFunc(20, Timer, 2);
		}
		break;
	case 3:
		if (button_e)
		{
			if (Map[5].revolutionAngle[1] > 0 && Map[6].revolutionAngle[1] < 0)
			{
				Map[5].revolutionAngle[1] -= 2.0f;
				Map[6].revolutionAngle[1] += 2.0f;
			}
			else if (Map[5].revolutionAngle[1] < 0 && Map[6].revolutionAngle[1] > 0)
			{
				Map[5].revolutionAngle[1] += 2.0f;
				Map[6].revolutionAngle[1] -= 2.0f;
			}
			if (Map[5].move_pos.x < Map[6].move_pos.x)
			{
				Map[5].revolutionAngle[1] = 0.0f;
				Map[6].revolutionAngle[1] = 0.0f;
				Map[5].move_pos.x += 0.01f;
				Map[6].move_pos.x -= 0.01f;
			}
			glutTimerFunc(20, Timer, 3);
		}
		else if (button_E) {
			if (Map[5].move_pos.x > -0.3 && Map[6].move_pos.x < 0.3)
			{
				Map[5].move_pos.x -= 0.01f;
				Map[6].move_pos.x += 0.01f;
			}
			glutTimerFunc(20, Timer, 3);
		}
		break;
	case 4:
		if (button_t)
		{
			Map[3].rotateAngle[0] -= 2.0f;
			Map[4].rotateAngle[0] += 2.0f;
			if (Map[3].rotateAngle[0] <= -60.0f) {
				button_t = FALSE;
				button_T = TRUE;

			}
			else if (Map[4].rotateAngle[0] >= 60.0f) {
				button_t = FALSE;
				button_T = TRUE;
			}
			glutTimerFunc(20, Timer, 4);
		}
		else if (button_T) {
			Map[3].rotateAngle[0] += 2.0f;
			Map[4].rotateAngle[0] -= 2.0f;
			if (Map[3].rotateAngle[0] >= 60.0f) {
				button_t = TRUE;
				button_T = FALSE;
			}
			else if (Map[4].rotateAngle[0] <= -60.0f) {
				button_t = TRUE;
				button_T = FALSE;
			}
			glutTimerFunc(20, Timer, 4);
		}
		break;
	case 5:
		if (button_y)
		{
			for (int i = 0; i < 3; ++i) {
				camera[i].CR_matrix = glm::rotate(camera[i].CR_matrix, glm::radians(2.0f), AXIS_Y);
			}


			glutTimerFunc(20, Timer, 5);
		}
		else if (button_Y) {
			for (int i = 0; i < 3; ++i) {
				camera[i].CR_matrix = glm::rotate(camera[i].CR_matrix, glm::radians(-2.0f), AXIS_Y);
			}
			glutTimerFunc(20, Timer, 5);
		}
		break;
	case 6:
		if (button_R) {
			if (g_rotate == 1) {
				Map[0].revolutionAngle[1] += 3.0f;;
				Map[1].revolutionAngle[1] += 3.0f;;
				Map[2].revolutionAngle[1] += 3.0f;;
			}
			if (g_rotate == 2) {
				Map[0].revolutionAngle[1] -= 3.0f;;
				Map[1].revolutionAngle[1] -= 3.0f;;
				Map[2].revolutionAngle[1] -= 3.0f;;
			}
			glutTimerFunc(20, Timer, 6);
		}
		break;
	default:
		break;
	}
}

GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'b':
		Map[1].move_pos.x += 0.1f;
		break;
	case 'B':
		Map[1].move_pos.x -= 0.1f;
		break;
	case 'm':
		button_m = !button_m;
		if (button_M) {
			button_M = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 1);
		break;
	case 'M':
		button_M = !button_M;
		if (button_m) {
			button_m = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 1);
		break;
	case 'f':
		button_f = !button_f;
		if (button_F) {
			button_F = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 2);
		break;
	case 'F':
		button_F = !button_F;
		if (button_f) {
			button_f = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 2);
		break;
	case 'e':
		button_e = !button_e;
		if (button_E) {
			button_E = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 3);
		break;
	case 'E':
		button_E = !button_E;
		if (button_e) {
			button_e = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 3);
		break;
	case 't':
		button_t = !button_t;
		if (button_T) {
			button_T = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 4);
		break;
	case 'T':
		button_T = !button_T;
		if (button_t) {
			button_t = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 4);
		break;
	case 'z':
		for (auto &a : camera)
			a.camerapos.z += 0.1f;
		break;
	case 'Z':for (auto& a : camera)
		a.camerapos.z -= 0.1f;
		break;
	case 'x':
		for (auto &a : camera)
			a.camerapos.x += 0.1f;
		break;
	case 'X':
		for (auto &a : camera)
			a.camerapos.x -= 0.1f;
		break;
	case 'y':
		button_y = !button_y;
		if (button_Y) {
			button_Y = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 5);
		break;
	case 'Y':
		button_Y = !button_Y;
		if (button_y) {
			button_y = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 5);
		break;
	case 'r':
		if (!button_R) {
			g_rotate = 1;
			button_R = true;
			glutTimerFunc(20, Timer, 6);
		}
		else {
			button_R = false;
		}
		break;
	case 'R':
		if (!button_R) {
			g_rotate = 2;
			button_R = true;
			glutTimerFunc(20, Timer, 6);
		}
		else {
			button_R = false;
		}
		break;
	case 's':
		button_m = FALSE;  button_M = FALSE;  button_f = FALSE;  button_F = FALSE;
		button_e = FALSE;  button_E = FALSE;  button_t = FALSE;  button_T = FALSE;
		button_y = FALSE;  button_Y = FALSE;  button_R = false;
		break;
	case 'c':
		for (int i = 1; i < 7; ++i) {
			Map[i].revolutionAngle[1] = 0;
			Map[i].rotateAngle[0] = 0;
			Map[i].rotateAngle[1] = 0;
		}
		Map[5].move_pos.x = -0.3;
		Map[6].move_pos.x = 0.3;

		for (auto &a : camera)
			a.CR_matrix = glm::mat4(1.0f);
		Map[1].move_pos.x = 0;
		break;
	case 'q':
		glutLeaveMainLoop();
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


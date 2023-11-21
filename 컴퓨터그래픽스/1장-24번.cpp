
#include "stdafx.h"
#include "player.h"
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
using namespace std;
using namespace glm;

vec3 tile_vertex[4] = { {-0.05,-0.5,0.05},{0.05,-0.5,0.05},{0.05,-0.5,-0.05} ,{-0.05,-0.5,-0.05} };

float WIDTH = 1000;
float HEIGHT = 1000;
int g_rotate = 0;
float maxRotationAngle = 30.0f;
int Robotspeed = 1;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
GLvoid KeyBoardFuncUP(unsigned char key, int x, int y);
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
enum move_type {
	UP = 1,
	LEFT,
	DOWN,
	RIGHT
};
std::vector <glm::vec3> D_vertex; std::vector <glm::vec3> D_normal; std::vector <glm::vec2> D_uv;
GLuint g_vbo;
GLvoid Timer(int value);

struct Dice {
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	vec3 color[6];
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 parents_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	bool open_button = FALSE;

	Dice(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		for (int i = 0; i < 6; ++i) {
			color[i].r = urd(dre);
			color[i].g = urd(dre);
			color[i].b = urd(dre);
		}
		move_pos.x += center.first;
		move_pos.z += center.second;

		move_pos.y += 0.5f;
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &g_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo); // VBO를 바인딩
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * D_vertex.size(), &D_vertex[0], GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		// VBO에 데이터 복사

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
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

		glUniform3f(colorUniformLoc, color[0].r, color[0].g, color[0].b);
		//아랫면
		glDrawArrays(GL_TRIANGLES, 6 * 0, 6);

		//윗면
		glUniform3f(colorUniformLoc, color[1].r, color[1].g, color[1].b);
		glDrawArrays(GL_TRIANGLES, 6 * 1, 6);

		//앞면
		if (!open_button) {
			glUniform3f(colorUniformLoc, color[2].r, color[2].g, color[2].b);
			glDrawArrays(GL_TRIANGLES, 6 * 2, 6);
		}
		//오른면
		glUniform3f(colorUniformLoc, color[3].r, color[3].g, color[3].b);
		glDrawArrays(GL_TRIANGLES, 6 * 3, 6);

		//뒷면
		glUniform3f(colorUniformLoc, color[4].r, color[4].g, color[4].b);
		glDrawArrays(GL_TRIANGLES, 6 * 4, 6);

		//왼쪽 면
		glUniform3f(colorUniformLoc, color[5].r, color[5].g, color[5].b);
		glDrawArrays(GL_TRIANGLES, 6 * 5, 6);

	}


};

struct TILE {
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	vec3 color;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 parents_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	

	TILE(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);

		color.r = urd(dre);
		color.g = urd(dre);
		color.b = urd(dre);
		move_pos.x += center.first;
		move_pos.z += center.second;
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &g_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo); // VBO를 바인딩
		glBufferData(GL_ARRAY_BUFFER, sizeof(tile_vertex), &tile_vertex, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		// VBO에 데이터 복사

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tile_vertex), &tile_vertex, GL_STATIC_DRAW);
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

		glUniform3f(colorUniformLoc, color.r, color.g, color.b);
		//아랫면
		glDrawArrays(GL_POLYGON, 0, 4);

	}


};
struct CAMERA {
	vec3 pos = { 0,0,1.8f };
	glm::mat4 camerapos = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 CR_matrix = glm::mat4(1.0f); //단위행렬로 초기화

	void Draw() {
		glm::vec3 cameraPos = { pos }; //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0, 0, 0); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- 투영 공간 설정: fovy, aspect, near, far
		//projection = glm::translate(projection, glm::vec3(0.0, 0.0, -2.0)); //--- 공간을 z축 이동
		projection = projection * CR_matrix; //--- 공간을 z축 이동
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
};
vector <Dice> dice;
vector <Dice> block;
CAMERA camera;
vector<gameObject> g_gameobjects;
player playerObject;
bool box1, box2, box3;
vector <TILE> tile;
bool check_collision(Dice& BLOCK) {
	// Check if the player is in the air (jumping)
	if (playerObject.m_position.y > -0.3f) {


		return false; // No collision while jumping
	}

	if ((BLOCK.move_pos.x - 0.1f < playerObject.m_position.x && playerObject.m_position.x < BLOCK.move_pos.x + 0.1f) &&
		(BLOCK.move_pos.z - 0.1f < playerObject.m_position.z && playerObject.m_position.z < BLOCK.move_pos.z + 0.1f) &&
		playerObject.m_position.y > -0.5f) {
		return true;  // Collision detected with the top surface of the cube


	}
	else {


		return false; // No collision with the top surface while not jumping
	}

}

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
	ReadObj("cube.obj", D_vertex, D_normal, D_uv);

	dice.emplace_back(WIDTH / 2, HEIGHT / 2);
	block.emplace_back(WIDTH / 2, HEIGHT / 2);
	block.emplace_back(WIDTH / 2, HEIGHT / 2);
	block.emplace_back(WIDTH / 2, HEIGHT / 2);
	glutTimerFunc(20, Timer, 7);
	for (int i = 0; i < 100; ++i) {
		tile.emplace_back(WIDTH / 2, HEIGHT / 2);
	}
	playerObject.AddChildObject(g_vbo, D_vertex.size(), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)).SetColor(glm::vec3(1, 0, 0));;	//body
	playerObject.AddChildObject(g_vbo, D_vertex.size(), glm::vec3(0, 0.5f, 0), glm::vec3(0, 0, 0), glm::vec3(0.5, 0.5, 0.5)).SetColor(glm::vec3(0, 1, 0));	//head
	playerObject.AddChildObject(g_vbo, D_vertex.size(), glm::vec3(0, 0.25f, 0.25), glm::vec3(0, 0, 0), glm::vec3(0.1, 0.1, 0.3)).SetColor(glm::vec3(0, 0, 1));	//head

	playerObject.AddChildObject(g_vbo, D_vertex.size(), glm::vec3(0.6, 0.0, 0), glm::vec3(0, 0, 0), glm::vec3(0.2, 1, 0.2)).SetColor(glm::vec3(0, 0, 1));	//left hand
	playerObject.AddChildObject(g_vbo, D_vertex.size(), glm::vec3(-0.6, 0.0f, 0), glm::vec3(0, 0, 0), glm::vec3(0.2, 1, 0.2)).SetColor(glm::vec3(0, 0, 1));	//right hand

	playerObject.AddChildObject(g_vbo, D_vertex.size(), glm::vec3(0.4, -1.0f, 0), glm::vec3(0, 0, 0), glm::vec3(0.3, 1, 0.3)).SetColor(glm::vec3(1, 0, 1));	//left leg
	playerObject.AddChildObject(g_vbo, D_vertex.size(), glm::vec3(-0.4, -1.0f, 0), glm::vec3(0, 0, 0), glm::vec3(0.3, 1, 0.3)).SetColor(glm::vec3(1, 0, 1));	//right leg
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(-0.4, 0.4);
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			tile[10 * i + j].move_pos.x = -0.45 + 0.1 * j;
			tile[10 * i + j].move_pos.z = -0.45 + 0.1 * i;
		}
	}
	playerObject.SetScale(0.1f, 0.1f, 0.1f);
	for (int i = 0; i < 3; ++i) {
		block[i].move_pos.x = urd(dre);
		block[i].move_pos.z = urd(dre);
		block[i].scale[0] = 0.1f;
		block[i].scale[1] = 0.1f;
		block[i].scale[2] = 0.1f;
		block[i].move_pos.y -= 0.9f;
	}
	dice[0].open_button = TRUE;
	//dice[2].move_pos.y = 1.0f * dice[1].scale[1];
	glutTimerFunc(20, Timer, 0);
	glutTimerFunc(20, Timer, 7);
	glutKeyboardFunc(KeyBoardFunc);
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutKeyboardUpFunc(KeyBoardFuncUP);
	glutReshapeFunc(Reshape);
	glutMainLoop(); // 이벤트 처리 시작
}

void drawScene()
{
	glClearColor(1, 1, 1, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	camera.Draw();


	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	dice[0].Draw();
	for (int i = 0; i < tile.size(); ++i) {
		tile[i].Draw();
	}
	block[0].Draw();
	block[1].Draw();
	block[2].Draw();
	playerObject.Draw(shaderProgramID);



	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glutSwapBuffers(); //화면에 출력
}
int TIMER = 20;
bool is_move[4] = { FALSE };
GLvoid Timer(int value)
{
	switch (value)
	{
	case 0:
		glutPostRedisplay();
		glutTimerFunc(20, Timer, 0);
		break;
	case 1:
		if (is_move[0]) {
			if (playerObject.m_position.z < -0.5) {
				playerObject.Rotate(Y, 0);
				playerObject.Move(Z, 0.005f);
				glutTimerFunc(TIMER, Timer, DOWN);
			}
			else {
				if (!check_collision(block[0])) {
					playerObject.Rotate(Y, 180);
					playerObject.Move(Z, -0.002f);
				}
				else {
					playerObject.Move(Z, 0.015f);
				}
				if (!check_collision(block[1]))
				{
					playerObject.Rotate(Y, 180);
					playerObject.Move(Z, -0.002f);
				}
				else {
					playerObject.Move(Z, 0.015f);
				}
				if (!check_collision(block[2]))
				{
					playerObject.Rotate(Y, 180);
					playerObject.Move(Z, -0.002f);
				}
				else {
					playerObject.Move(Z, 0.015f);
				}

				glutTimerFunc(TIMER, Timer, UP);
			}
			playerObject.Animate(maxRotationAngle, Robotspeed);
		}
		break;
	case 2:
		if (is_move[1]) {
			if (playerObject.m_position.x <= -0.5) {
				playerObject.Rotate(Y, 90);
				playerObject.Move(X, 0.005f);
				glutTimerFunc(TIMER, Timer, RIGHT);
			}
			else {
				if (!check_collision(block[0])) {
					playerObject.Rotate(Y, -90);
					playerObject.Move(X, -0.002f);
				}
				else {
					playerObject.Move(X, 0.015f);
				}
				if (!check_collision(block[1]))
				{
					playerObject.Rotate(Y, -90);
					playerObject.Move(X, -0.002f);
				}
				else {
					playerObject.Move(X, 0.015f);
				}
				if (!check_collision(block[2]))
				{
					playerObject.Rotate(Y, -90);
					playerObject.Move(X, -0.002f);
				}
				else {
					playerObject.Move(X, 0.015f);
				}
				glutTimerFunc(TIMER, Timer, LEFT);
			}

			playerObject.Animate(maxRotationAngle, Robotspeed);

		}
		break;
	case 3:
		if (is_move[2]) {
			if (playerObject.m_position.z > 0.5) {
				playerObject.Rotate(Y, 180);
				playerObject.Move(Z, -0.005f);
				glutTimerFunc(TIMER, Timer, UP);
			}
			else {
				if (!check_collision(block[0])) {
					playerObject.Rotate(Y, 0);
					playerObject.Move(Z, 0.002f);
				}
				else {
					playerObject.Move(Z, -0.015f);
				}
				if (!check_collision(block[1]))
				{
					playerObject.Rotate(Y, 0);
					playerObject.Move(Z, 0.002f);
				}
				else {
					playerObject.Move(Z, -0.015f);
				}
				if (!check_collision(block[2]))
				{
					playerObject.Rotate(Y, 0);
					playerObject.Move(Z, 0.002f);
				}
				else {
					playerObject.Move(Z, -0.015f);
				}
				glutTimerFunc(TIMER, Timer, DOWN);
			}
			playerObject.Animate(maxRotationAngle, Robotspeed);
		}

		break;
	case 4:
		if (is_move[3]) {
			if (playerObject.m_position.x > 0.5) {
				playerObject.Rotate(Y, -90);
				playerObject.Move(X, -0.005f);
				glutTimerFunc(TIMER, Timer, LEFT);
			}
			else {
				if (!check_collision(block[0])) {
					playerObject.Rotate(Y, 90);
					playerObject.Move(X, 0.002f);
				}
				else {
					playerObject.Move(X, -0.015f);
				}
				if (!check_collision(block[1]))
				{
					playerObject.Rotate(Y, 90);
					playerObject.Move(X, 0.002f);
				}
				else {
					playerObject.Move(X, -0.015f);
				}
				if (!check_collision(block[2]))
				{
					playerObject.Rotate(Y, 90);
					playerObject.Move(X, 0.002f);
				}
				else {
					playerObject.Move(X, -0.015f);
				}
				glutTimerFunc(TIMER, Timer, RIGHT);
			}
			playerObject.Animate(maxRotationAngle, Robotspeed);
		}
		break;

	case 5:
		if (!playerObject.check_jump) {
			if (playerObject.m_position.y < -0.1f)
			{
				playerObject.m_position.y += 0.01f;
				glutTimerFunc(TIMER, Timer, 5);
			}
			else {
				playerObject.check_jump = TRUE;
				glutTimerFunc(TIMER, Timer, 5);
			}

		}
		else
		{

			if ((block[0].move_pos.x - 0.1f < playerObject.m_position.x && playerObject.m_position.x < block[0].move_pos.x + 0.1f) &&
				(block[0].move_pos.z - 0.1f < playerObject.m_position.z && playerObject.m_position.z < block[0].move_pos.z + 0.1f) &&
				playerObject.m_position.y > -0.5f) {
				if (playerObject.m_position.y > -0.2f)
				{
					box1 = TRUE;
					playerObject.m_position.y -= 0.01f;
					glutTimerFunc(TIMER, Timer, 5);
				}
			}
			else if ((block[1].move_pos.x - 0.1f < playerObject.m_position.x && playerObject.m_position.x < block[1].move_pos.x + 0.1f) &&
				(block[1].move_pos.z - 0.1f < playerObject.m_position.z && playerObject.m_position.z < block[1].move_pos.z + 0.1f) &&
				playerObject.m_position.y > -0.5f) {
				if (playerObject.m_position.y > -0.2f)
				{
					box2 = TRUE;
					playerObject.m_position.y -= 0.01f;
					glutTimerFunc(TIMER, Timer, 5);
				}
			}
			else if ((block[2].move_pos.x - 0.1f < playerObject.m_position.x && playerObject.m_position.x < block[2].move_pos.x + 0.1f) &&
				(block[2].move_pos.z - 0.1f < playerObject.m_position.z && playerObject.m_position.z < block[2].move_pos.z + 0.1f) &&
				playerObject.m_position.y > -0.5f) {
				if (playerObject.m_position.y > -0.2f)
				{
					box3 = TRUE;
					playerObject.m_position.y -= 0.01f;
					glutTimerFunc(TIMER, Timer, 5);
				}
			}
			else {
				if (playerObject.m_position.y > -0.3f)
				{
					playerObject.m_position.y -= 0.01f;
					glutTimerFunc(TIMER, Timer, 5);
				}
			}

		}
		break;
	case 6:
		if (button_Y) {
			if (g_rotate == 1) {
				dice[0].revolutionAngle[1] += 3.0f;
				block[0].revolutionAngle[1] += 3.0f;
				block[1].revolutionAngle[1] += 3.0f;
				block[2].revolutionAngle[1] += 3.0f;
				playerObject.m_rotate.y += 3.0f;

			}
			if (g_rotate == 2) {
				dice[0].revolutionAngle[1] -= 3.0f;
				block[0].revolutionAngle[1] -= 3.0f;
				block[1].revolutionAngle[1] -= 3.0f;
				block[2].revolutionAngle[1] -= 3.0f;
				playerObject.m_rotate.y -= 3.0f;

			}
			glutTimerFunc(20, Timer, 6);
		}
		break;
	case 7:
		if (box1)

		{
			if (playerObject.m_position.y <= -0.2f) {
				if (block[0].move_pos.y > -0.5f) {
					block[0].move_pos.y -= 0.01f;
					playerObject.m_position.y -= 0.01f;
				}
			}
		}
		if (box2)

		{
			if (playerObject.m_position.y <= -0.2f) {
				if (block[1].move_pos.y > -0.5f) {
					block[1].move_pos.y -= 0.01f;
					playerObject.m_position.y -= 0.01f;
				}
			}
		}
		if (box3)

		{
			if (playerObject.m_position.y <= -0.2f) {
				if (block[2].move_pos.y > -0.5f) {
					block[2].move_pos.y -= 0.01f;
					playerObject.m_position.y -= 0.01f;
				}
				
			}
		}
		glutTimerFunc(TIMER, Timer, 7);
		break;
	default:
		break;
	}
}

GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{

	switch (key)
	{

	case 'z':
		camera.pos.z += 0.1f;
		break;

	case 'Z':
		camera.pos.z -= 0.1f;
		break;
	case 'x':
		camera.pos.x += 0.1f;
		break;

	case 'X':
		camera.pos.x -= 0.1f;
		break;
	case 'y':
		if (!button_Y) {
			g_rotate = 1;
			button_Y = true;
			glutTimerFunc(20, Timer, 6);
		}
		else {
			button_Y = false;
		}
		break;
	case 'Y':
		if (!button_Y) {
			g_rotate = 2;
			button_Y = true;
			glutTimerFunc(20, Timer, 6);
		}
		else {
			button_Y = false;
		}
		break;
	case 'w':
		if (!is_move[0]) {
			is_move[0] = true;
			glutTimerFunc(20, Timer, UP);
		}
		if (playerObject.m_position.z < block[0].move_pos.z - 0.1f && playerObject.m_position.y > -0.3 && box1)
		{
			playerObject.m_position.y -= 0.02f;
		}
		else if (playerObject.m_position.z < block[1].move_pos.z - 0.1f && playerObject.m_position.y > -0.3 && box2)
		{
			playerObject.m_position.y -= 0.02f;
		}
		else if (playerObject.m_position.z < block[2].move_pos.z - 0.1f && playerObject.m_position.y > -0.3 && box3)
		{
			playerObject.m_position.y -= 0.02f;
		}
		break;
	case 'a':
		if (!is_move[1]) {
			is_move[1] = true;
			glutTimerFunc(20, Timer, LEFT);
		}
		if (playerObject.m_position.x < block[0].move_pos.x - 0.1f && playerObject.m_position.y > -0.3 && box1)
		{
			playerObject.m_position.y -= 0.02f;
		}
		else if (playerObject.m_position.x < block[1].move_pos.x - 0.1f && playerObject.m_position.y > -0.3 && box2)
		{
			playerObject.m_position.y -= 0.02f;
		}
		else if (playerObject.m_position.x < block[2].move_pos.x - 0.1f && playerObject.m_position.y > -0.3 && box3)
		{
			playerObject.m_position.y -= 0.02f;
		}

		break;
	case 's':
		if (!is_move[2]) {
			is_move[2] = true;
			glutTimerFunc(20, Timer, DOWN);
		}
		if (playerObject.m_position.z > block[0].move_pos.z + 0.1f && playerObject.m_position.y > -0.3 && box1)
		{
			playerObject.m_position.y -= 0.02f;
		}
		else if (playerObject.m_position.z > block[1].move_pos.z + 0.1f && playerObject.m_position.y > -0.3 && box2)
		{
			playerObject.m_position.y -= 0.02f;
		}
		else if (playerObject.m_position.z > block[2].move_pos.z + 0.1f && playerObject.m_position.y > -0.3 && box3)
		{
			playerObject.m_position.y -= 0.02f;
		}
		break;
	case 'd':
		if (!is_move[3]) {
			is_move[3] = true;
			glutTimerFunc(20, Timer, RIGHT);
		}
		if (playerObject.m_position.x > block[0].move_pos.x + 0.1f && playerObject.m_position.y > -0.3 && box1)
		{
			playerObject.m_position.y -= 0.02f;
		}
		else if (playerObject.m_position.x > block[1].move_pos.x + 0.1f && playerObject.m_position.y > -0.3 && box2)
		{
			playerObject.m_position.y -= 0.02f;
		}
		else if (playerObject.m_position.x > block[2].move_pos.x + 0.1f && playerObject.m_position.y > -0.3 && box3)
		{
			playerObject.m_position.y -= 0.02f;
		}
		break;
	case 32:		//스페이스바
		box1 = FALSE;
		box2 = FALSE;
		box3 = FALSE;

		playerObject.check_jump = FALSE;
		glutTimerFunc(20, Timer, 5);
		break;

	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();

}
GLvoid KeyBoardFuncUP(unsigned char key, int x, int y)
{
	for (int i = 0; i < 4; ++i) {
		is_move[i] = FALSE;;
	}

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


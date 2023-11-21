
#include "stdafx.h"
#include "player.h"
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
using namespace std;
using namespace glm;

float WIDTH = 1000;
float HEIGHT = 1000;
int g_rotate = 0;
float maxRotationAngle = 30.0f;
int Robotspeed = 1;
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

bool button_y = FALSE; bool button_Y = FALSE;
pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}
std::vector <glm::vec3> D_vertex; std::vector <glm::vec3> D_normal; std::vector <glm::vec2> D_uv;
GLuint g_vbo;
GLvoid Timer(int value);
GLvoid Motion(int x, int y);
GLvoid ClickFunc(int button, int state, int x, int y);
bool button_LeftRotate;
bool button_RightRotate;
bool open_bottom;
struct Dice {
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	vec3 color[6];
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	bool open_button = FALSE;
	float speed;
	Dice(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		uniform_int_distribution<int> num(3, 7);
		uniform_int_distribution<int> z_pos(-1, 1);
		for (int i = 0; i < 6; ++i) {
			color[i].r = urd(dre);
			color[i].g = urd(dre);
			color[i].b = urd(dre);
		}
		float size = num(dre);
		scale[0] = 0.01 * size; scale[1] = 0.01 * size; scale[2] = 0.01 * size;
		speed = (size - 2) * 0.01;

		move_pos.x += center.first;


		move_pos.y += 0.5f;

		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &g_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo); // VBO를 바인딩
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

		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[1]), AXIS_Y);
		model_matrix = glm::translate(model_matrix, (move_pos));
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);
		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
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
		if (open_bottom) {
			if ((45 <= revolutionAngle[2] && revolutionAngle[2] < 135) || (-315 <= revolutionAngle[2] && revolutionAngle[2] < -225)) {

			}
			else {
				glUniform3f(colorUniformLoc, color[5].r, color[5].g, color[5].b);
				glDrawArrays(GL_TRIANGLES, 6 * 5, 6);
			}
		}
		else {
			glUniform3f(colorUniformLoc, color[5].r, color[5].g, color[5].b);
			glDrawArrays(GL_TRIANGLES, 6 * 5, 6);
		}

	}

	void falling() {

		{

			if ((90 <= revolutionAngle[2] && revolutionAngle[2] < 270) || (-270 <= revolutionAngle[2] && revolutionAngle[2] < -90)) {
				if (open_bottom) {
					move_pos.y += speed;
				}
				else if (move_pos.y < 0.5)
					move_pos.y += speed;
			}
			else {
				if (open_bottom) {
					move_pos.y -= speed;
				}
				else if (move_pos.y > -0.5 + scale[1] * 1.0f)
					move_pos.y -= speed;
			}

			if ((0 < revolutionAngle[2] && revolutionAngle[2] < 180) || (-360 < revolutionAngle[2] && revolutionAngle[2] < -180)) {
				if(open_bottom){
					move_pos.x -= speed;
				}
				else if (move_pos.x > -0.5 + scale[1] * 0.5) {
					move_pos.x -= speed;
				}
			}
			else if ((180 < revolutionAngle[2] && revolutionAngle[2] < 360) || (-180 <= revolutionAngle[2] && revolutionAngle[2] < 0)) {
				if (open_bottom) {
					move_pos.x += speed;
				}
				else if (move_pos.x < 0.5 - scale[1] * 0.5)
					move_pos.x += speed;
			}



		}

	}
};
int Ball_num = 0;
struct SPHERE {
	GLUquadricObj* qobj;
	float radian = 0.05f;

	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale = 1.0f;
	vec3 speed;
	vec3 color;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 parents_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	SPHERE(int x, int y)
	{
		Ball_num++;
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		uniform_real_distribution<float> SPEED(0, 0.02);
		color.r = urd(dre);
		color.g = urd(dre);
		color.b = urd(dre);
		move_pos.x = center.first;
		move_pos.y = center.second;
		speed = { (SPEED(dre)),(SPEED(dre)) ,(SPEED(dre)) };


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
		gluQuadricDrawStyle(qobj, GLU_FILL);


		gluSphere(qobj, radian, 50, 50);
	}

	void Move()
	{
		move_pos.x += speed.x;
		move_pos.y += speed.y;
		move_pos.z += speed.z;
		
			if (move_pos.x >= 0.45 || move_pos.x <= -0.45)
				speed.x *= -1;
			if (move_pos.y >= 0.45 || move_pos.y <= -0.45)
				speed.y *= -1;
			if (move_pos.z >= 0.45 || move_pos.z <= -0.45)
				speed.z *= -1;
		
	}
};
struct CAMERA {
	vec3 pos = { 0,0, 2.0f };
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
vector <SPHERE> circle;

//bool check_collision(Dice& BLOCK) {
//	// Check if the player is in the air (jumping)
//	if (playerObject.m_position.y > -0.3f) {
//
//
//		return false; // No collision while jumping
//	}
//
//	if ((BLOCK.move_pos.x - 0.1f < playerObject.m_position.x && playerObject.m_position.x < BLOCK.move_pos.x + 0.1f) &&
//		(BLOCK.move_pos.z - 0.1f < playerObject.m_position.z && playerObject.m_position.z < BLOCK.move_pos.z + 0.1f) &&
//		playerObject.m_position.y > -0.5f) {
//		return true;  // Collision detected with the top surface of the cube
//
//
//	}
//	else {
//
//
//		return false; // No collision with the top surface while not jumping
//	}
//
//}

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
	dice[0].open_button = TRUE;
	dice[0].scale[0] = 1; dice[0].scale[1] = 1; dice[0].scale[2] = 1;
	block.emplace_back(WIDTH / 2, HEIGHT / 2);
	block.emplace_back(WIDTH / 2, HEIGHT / 2);
	block.emplace_back(WIDTH / 2, HEIGHT / 2);
	//block.emplace_back(WIDTH / 2, HEIGHT / 2);
	circle.emplace_back(WIDTH / 2, HEIGHT / 2);

	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(-0.4, 0.4);
	uniform_real_distribution<float> z_pos(-0.5, 0.5);
	for (int i = 0; i < block.size(); ++i) {
		block[i].move_pos.z = z_pos(dre);
	}
	//dice[2].move_pos.y = 1.0f * dice[1].scale[1];
	glutTimerFunc(20, Timer, 0);
	glutTimerFunc(20, Timer, 1);
	glutTimerFunc(20, Timer, 2);
	glutKeyboardFunc(KeyBoardFunc);
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수

	glutMouseFunc(ClickFunc);
	glutMotionFunc(Motion);
	glutReshapeFunc(Reshape);
	glutMainLoop(); // 이벤트 처리 시작
}

void drawScene()
{
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	camera.Draw();


	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	for (int i = 0; i < circle.size(); ++i) {
		circle[i].Draw();
	}
	dice[0].Draw();
	for (int i = 0; i < block.size(); ++i) {
		block[i].Draw();
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glutSwapBuffers(); //화면에 출력
}
int TIMER = 20;
bool is_move = { FALSE };

bool click;
GLvoid Timer(int value)
{
	switch (value)
	{
	case 0:
		glutPostRedisplay();
		for (int i = 0; i < circle.size(); ++i) {
			circle[i].Move();
		}
		for (int i = 0; i < block.size(); ++i) {
			block[i].falling();
		}
		glutTimerFunc(20, Timer, 0);
		break;
	case 1:
		if (button_Y) {
			if (g_rotate == 1) {
				dice[0].revolutionAngle[1] += 3.0f;
			}
			if (g_rotate == 2) {
				dice[0].revolutionAngle[1] -= 3.0f;
			}
			glutTimerFunc(20, Timer, 1);
		}
		break;
	case 2:
		if (click)
		{
			if (button_LeftRotate)
			{
				if (dice[0].revolutionAngle[2] >= 360)
				{
					dice[0].revolutionAngle[2] = 0.0f;
				}
				dice[0].revolutionAngle[2] += 9.0f;
				for (int i = 0; i < circle.size(); ++i) {
					circle[i].revolutionAngle[2] = dice[0].revolutionAngle[2];
				}
				for (int i = 0; i < block.size(); ++i) {
					block[i].revolutionAngle[2] = dice[0].revolutionAngle[2];
				}
			}
			if (button_RightRotate)
			{
				if (dice[0].revolutionAngle[2] <= -360)
				{
					dice[0].revolutionAngle[2] = 0.0f;
				}
				dice[0].revolutionAngle[2] -= 9.0f;
				for (int i = 0; i < circle.size(); ++i) {
					circle[i].revolutionAngle[2] = dice[0].revolutionAngle[2];
				}
				for (int i = 0; i < block.size(); ++i) {
					block[i].revolutionAngle[2] = dice[0].revolutionAngle[2];

				}

			}

		}
		glutTimerFunc(20, Timer, 2);
		break;
	default:
		break;
	}
}

GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(400, 600);
	switch (key)
	{
	case 'o':
		open_bottom = !open_bottom;
		break;
	case 'b':
	{
		if (Ball_num < 5) {
			circle.emplace_back(urd(dre), urd(dre));
		}

	}
	break;
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
			glutTimerFunc(20, Timer, 1);
		}
		else {
			button_Y = false;
		}
		break;
	case 'Y':
		if (!button_Y) {
			g_rotate = 2;
			button_Y = true;
			glutTimerFunc(20, Timer, 1);
		}
		else {
			button_Y = false;
		}
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();

}

float mouse_posX;
float mouse_posY;

GLvoid ClickFunc(int button, int state, int x, int y)
{
	auto pos = ConvertWinToGL(x, y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		mouse_posX = pos.first;
		mouse_posY = pos.second;
		click = TRUE;

	}
	if (state == GLUT_UP)
	{
		click = FALSE;
	}
}
GLvoid Motion(int x, int y)
{
	auto move_pos = ConvertWinToGL(x, y);

	if (move_pos.first < mouse_posX)
	{
		button_LeftRotate = TRUE;
		button_RightRotate = FALSE;
	}
	else if (move_pos.first > mouse_posX) {
		button_LeftRotate = FALSE;
		button_RightRotate = TRUE;
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


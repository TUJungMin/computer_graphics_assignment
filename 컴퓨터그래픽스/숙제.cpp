#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#define MIN_DIFF_FLT 0.00001f
using namespace std;
using namespace glm;


float WIDTH = 1000;
float HEIGHT = 800;
float TIMER = 1;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

void make_vertexShaders();

void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
GLvoid TimerFunc(int value);
//--- 필요한 변수 선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
GLint result;
GLchar errorLog[512];
vec3 Line_vertex[2] = { {0.0f,0.0f,0.0f },{0.0f,0.0f,0.0f} };
vec3 basket_vertex[2] = { {-0.3f,0.0f,0.0f },{0.3f,0.0f,0.0f} };
vec3 Triangle_vertex[3] = {
	{-0.15f, -0.15f, 0.0f},  // 왼쪽 아래 꼭짓점
	{0.15f, -0.15f, 0.0f },   // 오른쪽 아래 꼭짓점
	{0.0f, 0.15f, 0.0f}     // 위 꼭짓점
};
vec3 Rect_vertex[4] = {
	{-0.15f,-0.15f,0.0f},
	{0.15f,-0.15f,0.0f},
	{0.15f,0.15f,0.0f},				//Rect_vertex[2],[3].x움직여야함
	{-0.15f,0.15f,0.0f}
};
vec3 Pentagon_vertex[5] = {
	{-0.2f,0.1f,0.0f},
	{-0.1f,-0.1f,0.0f},
	{0.1f,-0.1f,0.0f},
	{0.2f,0.1f,0.0f},
	{0.0f,0.3f,0.0f}			//Rect_vertex[2],[3].x움직여야함
};
int point[2];
int cnt = 0;
bool button_s = FALSE;
bool button_l = FALSE;
GLvoid Timer(int value);
GLvoid Motion(int x, int y);
GLvoid ClickFunc(int button, int state, int x, int y);
struct Information {
	vec3 color;
	float y_speed;
	float x_speed;
	float gravity = 9.8f;
};
pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}
void make_polygon(Information information);

struct Triangle {
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	GLuint m_vbo;
	Information info;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	float deltaTime = 0;
	int direction;
	vec3 first_position;
	std::vector<vec3> pathVertices;
	Triangle(int x, int y)
	{
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> Xspeed(1.0, 1.5);
		uniform_real_distribution<float> Yspeed(3, 5);
		uniform_real_distribution<float> urd(0, 1);
		uniform_int_distribution<int> dir(1, 2);

		auto center = ConvertWinToGL(x, y);
		info.color.r = urd(dre);
		info.color.g = urd(dre);
		info.color.b = urd(dre);
		direction = dir(dre);
		move_pos.x += center.first;
		move_pos.y += center.second;
		first_position.x = move_pos.x;
		first_position.y = move_pos.y;
		info.x_speed = Xspeed(dre);
		info.y_speed = Yspeed(dre);

		if (direction == 2) {
			move_pos.x += 2.5f;
			first_position.x += 2.5f;
		}
		InitVbo();
	}
	void InitVbo() {

		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle_vertex), Triangle_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, info.color.x, info.color.y, info.color.z);


		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::translate(model_matrix, (move_pos));
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);
		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glLineWidth(3.0f);
		if (!button_s)
			glDrawArrays(GL_POLYGON, 0, 3);
		else
			glDrawArrays(GL_LINE_LOOP, 0, 3);



	}

	void falling()
	{
		move_pos.y = first_position.y + 0.5 * (info.y_speed * deltaTime - (0.5f * info.gravity * deltaTime * deltaTime));

		if (direction == 2) {
			move_pos.x = first_position.x - info.x_speed * deltaTime;
		}
		else {
			move_pos.x = first_position.x + info.x_speed * deltaTime;;
		}
		pathVertices.push_back(move_pos);
	}


};
struct Line {

	vec3 move_pos = { 0.0f,0.0f,0.0f };
	vec3 color;
	GLuint m_vbo;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	void InitVbo() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);

		color.r = urd(dre);
		color.g = urd(dre);
		color.b = urd(dre);
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Line_vertex), Line_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Line_vertex), Line_vertex, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);

		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::translate(model_matrix, (move_pos));
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);
		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glLineWidth(3.0f);
		glDrawArrays(GL_LINES, 0, 2);

	}


};
struct Rectan {
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	GLuint m_vbo;
	Information info;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	float deltaTime = 0;
	int direction;
	vec3 first_position;
	std::vector<vec3> pathVertices;
	Rectan(int x, int y)
	{
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> Xspeed(1.0, 1.5);
		uniform_real_distribution<float> Yspeed(3, 5);
		uniform_real_distribution<float> urd(0, 1);
		uniform_int_distribution<int> dir(1, 2);

		auto center = ConvertWinToGL(x, y);
		info.color.r = urd(dre);
		info.color.g = urd(dre);
		info.color.b = urd(dre);
		direction = dir(dre);
		move_pos.x += center.first;
		move_pos.y += center.second;
		first_position.x = move_pos.x;
		first_position.y = move_pos.y;
		info.x_speed = Xspeed(dre);
		info.y_speed = Yspeed(dre);

		if (direction == 2) {
			move_pos.x += 2.5f;
			first_position.x += 2.5f;
		}
		InitVbo();
	}
	void InitVbo() {

		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Rect_vertex), Rect_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, info.color.x, info.color.y, info.color.z);


		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::translate(model_matrix, (move_pos));
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);
		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glLineWidth(3.0f);
		if (!button_s)
			glDrawArrays(GL_POLYGON, 0, 4);
		else
			glDrawArrays(GL_LINE_LOOP, 0, 4);



	}

	void falling()
	{
		move_pos.y = first_position.y + 0.5 * (info.y_speed * deltaTime - (0.5f * info.gravity * deltaTime * deltaTime));

		if (direction == 2) {
			move_pos.x = first_position.x - info.x_speed * deltaTime;
		}
		else {
			move_pos.x = first_position.x + info.x_speed * deltaTime;;
		}
		pathVertices.push_back(move_pos);
	}

};
struct Pentagon {
	vec3 move_pos = { 0.0f,0.0f,0.0f };

	GLuint m_vbo;
	Information info;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화

	float deltaTime = 0;
	int direction;
	vec3 first_position;
	vector <vec3> pathVertices;
	vec3 path;
	Pentagon(int x, int y)
	{
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> Xspeed(1.0, 1.5);
		uniform_real_distribution<float> Yspeed(3, 5);
		uniform_real_distribution<float> urd(0, 1);
		uniform_int_distribution<int> dir(1, 2);
		auto center = ConvertWinToGL(x, y);
		direction = dir(dre);
		info.color.r = urd(dre);
		info.color.g = urd(dre);
		info.color.b = urd(dre);
		move_pos.x += center.first;
		path.x = move_pos.x;
		path.y = move_pos.y;
		move_pos.y += center.second;
		first_position.x = move_pos.x;
		first_position.y = move_pos.y;
		info.x_speed = Xspeed(dre);
		info.y_speed = Yspeed(dre);
		if (direction == 2) {
			move_pos.x += 2.5f;
			first_position.x += 2.5f;
		}

		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Pentagon_vertex), Pentagon_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Pentagon_vertex), Pentagon_vertex, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, info.color.x, info.color.y, info.color.z);

		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::translate(model_matrix, (move_pos));
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);
		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glLineWidth(3.0f);
		if (!button_s)
			glDrawArrays(GL_POLYGON, 0, 5);
		else
			glDrawArrays(GL_LINE_LOOP, 0, 5);

	}


	void falling()
	{
		move_pos.y = 0.5 * (info.y_speed * deltaTime - (0.5f * info.gravity * deltaTime * deltaTime));

		if (direction == 2) {
			move_pos.x = first_position.x - info.x_speed * deltaTime;
		}
		else
			move_pos.x = first_position.x + info.x_speed * deltaTime;;
		pathVertices.push_back(move_pos);
		;
	}

};
void Collision_Check(float x, float y, float x1, float y1, float polygon_x1, float polygon_y1, float polygon_x2, float polygon_y2);
struct POLYGON {
	vector <vec3> vertex;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	Information info;
	GLuint m_vbo;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	bool fall = TRUE;
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	float deltaTime = 0;
	int direction;
	vec3 first_position;
	vector <vec3> pathVertices;
	//vec3 path;
	POLYGON(vector <vec3>& ver)
	{
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> Xspeed(1.0, 1.5);
		uniform_real_distribution<float> Yspeed(3, 5);
		uniform_real_distribution<float> urd(0, 1);
		uniform_int_distribution<int> dir(1, 2);
		vertex = ver;
		//direction = dir(dre);
		info.color.r = 1;
		info.color.g = 1;
		info.color.b = 1;
		//path.x = move_pos.x;
		//path.y = move_pos.y;
		first_position.x = move_pos.x;
		first_position.y = move_pos.y;
		info.x_speed = Xspeed(dre);
		info.y_speed = Yspeed(dre);
		InitVbo();
	}

	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(vec3), vertex.data(), GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Pentagon_vertex), Pentagon_vertex, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, info.color.x, info.color.y, info.color.z);

		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::translate(model_matrix, (move_pos));
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);
		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glLineWidth(3.0f);
		if (!button_s)
			glDrawArrays(GL_POLYGON, 0, vertex.size());
		else
			glDrawArrays(GL_LINE_LOOP, 0, vertex.size());

	}


	void falling()
	{
		if (fall) {
			move_pos.y = 0.5 * (info.y_speed * deltaTime - (0.5f * info.gravity * deltaTime * deltaTime));

			if (direction == 2) {
				move_pos.x = -info.x_speed * deltaTime;
			}
			else
				move_pos.x = info.x_speed * deltaTime;;
		}
	}

};
struct BASKET {
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	vec3 color;
	GLuint m_vbo;
	bool toggle_move = FALSE;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	void InitVbo() {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		move_pos.y = -0.8;
		color.r = urd(dre);
		color.g = urd(dre);
		color.b = urd(dre);
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(basket_vertex), basket_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(basket_vertex), basket_vertex, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);

		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::translate(model_matrix, (move_pos));
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);
		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);
	}
	void Move()
	{
		if (!toggle_move)
		{
			move_pos.x += 0.01f;
			if (move_pos.x > 0.7f)
			{
				toggle_move = TRUE;
			}

		}
		else
		{
			move_pos.x -= 0.01f;
			if (move_pos.x < -0.7f)
			{
				toggle_move = FALSE;
			}
		}
	}

};
Line Knife;
BASKET Basket;
vector <vec3> g_vertex;
vector <Line> g_line;
vector <Triangle> g_Triangle;
vector <Rectan> g_Rectangle;
vector<Pentagon> g_pentagon;
vector <POLYGON> g_polygon;
void DrawPath() {
	if (button_l) {
		GLuint m_vbo;
		glGenBuffers(1, &m_vbo); // VBO 생성

		for (int i = 0; i < g_Rectangle.size(); ++i) {
			if (g_Rectangle[i].pathVertices.size() != 0) {
				// Reset model matrix for path drawing
				unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
				glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

				// Bind the VBO for path drawing
				glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
				glBufferData(GL_ARRAY_BUFFER, g_Rectangle[i].pathVertices.size() * sizeof(vec3), g_Rectangle[i].pathVertices.data(), GL_STATIC_DRAW);
				int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
				glUniform3f(colorUniformLoc, 0, 0, 0);
				// Specify the attribute pointer for the position
				int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
				glEnableVertexAttribArray(vertexAttribLoc);
				glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

				// Draw the path
				glDrawArrays(GL_LINE_STRIP, 0, g_Rectangle[i].pathVertices.size());
			}
		}

		for (int i = 0; i < g_pentagon.size(); ++i) {
			if (g_pentagon[i].pathVertices.size() != 0) {
				// Reset model matrix for path drawing
				unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
				glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

				// Bind the VBO for path drawing
				glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
				glBufferData(GL_ARRAY_BUFFER, g_pentagon[i].pathVertices.size() * sizeof(vec3), g_pentagon[i].pathVertices.data(), GL_STATIC_DRAW);
				int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
				glUniform3f(colorUniformLoc, 0, 0, 0);
				// Specify the attribute pointer for the position
				int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
				glEnableVertexAttribArray(vertexAttribLoc);
				glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

				// Draw the path
				glDrawArrays(GL_LINE_STRIP, 0, g_pentagon[i].pathVertices.size());
			}
		}

		for (int i = 0; i < g_Triangle.size(); ++i) {
			if (g_Triangle[i].pathVertices.size() != 0) {
				// Reset model matrix for path drawing
				unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
				glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

				// Bind the VBO for path drawing
				glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
				glBufferData(GL_ARRAY_BUFFER, g_Triangle[i].pathVertices.size() * sizeof(vec3), g_Triangle[i].pathVertices.data(), GL_STATIC_DRAW);
				int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
				glUniform3f(colorUniformLoc, 0, 0, 0);
				// Specify the attribute pointer for the position
				int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
				glEnableVertexAttribArray(vertexAttribLoc);
				glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

				// Draw the path
				glDrawArrays(GL_LINE_STRIP, 0, g_Triangle[i].pathVertices.size());
			}
		}
	}
}
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(200, 200); // 윈도우의 위치 지정
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
	Knife.InitVbo();
	Basket.InitVbo();
	glutMouseFunc(ClickFunc);
	glutMotionFunc(Motion);
	glutTimerFunc(20, Timer, 0);
	glutTimerFunc(1000, Timer, 1);
	glutTimerFunc(1500, Timer, 2);
	glutTimerFunc(2000, Timer, 3);
	glutTimerFunc(20, Timer, 4);
	glutTimerFunc(20, Timer, 5);
	glutTimerFunc(20, Timer, 6);
	glutKeyboardFunc(KeyBoardFunc);
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);

	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene()
{

	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);

	
	for (int i = 0; i < g_Triangle.size(); ++i) {

		g_Triangle[i].Draw();
		if (g_Triangle[i].move_pos.y <= -2)
		{
			g_Triangle.erase(g_Triangle.begin() + i);
		}

	}
	for (int i = 0; i < g_Rectangle.size(); ++i) {
		g_Rectangle[i].Draw();
		if (g_Rectangle[i].move_pos.y <= -2)
		{
			g_Rectangle.erase(g_Rectangle.begin() + i);
		}

	}
	for (int i = 0; i < g_pentagon.size(); ++i) {

		g_pentagon[i].Draw();
		if (g_pentagon[i].move_pos.y <= -2)
		{
			g_pentagon.erase(g_pentagon.begin() + i);
		}

	}
	for (int i = 0; i < g_polygon.size(); ++i) {

		g_polygon[i].Draw();
		if (g_polygon[i].move_pos.y <= -2 || g_polygon[i].deltaTime > 10 )
		{
			g_polygon.erase(g_polygon.begin() + i);
		}

	}
	DrawPath();
	Basket.Draw();
	Knife.Draw();

	glutSwapBuffers(); //--- 화면에 출력하기
}
pair<float, float> pos;
GLvoid ClickFunc(int button, int state, int x, int y)
{
	auto mouseuppos = ConvertWinToGL(x, y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		Line_vertex[0].x = mouseuppos.first;
		Line_vertex[0].y = mouseuppos.second;
		Line_vertex[1].x = mouseuppos.first;
		Line_vertex[1].y = mouseuppos.second;


	}
	if (state == GLUT_UP)
	{
		int num = g_polygon.size();
		for (int i = 0; i < num; ++i) {
			if (g_polygon[i].fall) {
				for (int j = 0; j < g_polygon[i].vertex.size() - 1; ++j) {
					g_vertex.emplace_back(g_polygon[i].move_pos + g_polygon[i].vertex[j]);
					Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
						, g_polygon[i].move_pos.x + g_polygon[i].vertex[j].x, g_polygon[i].move_pos.y + g_polygon[i].vertex[j].y
						, g_polygon[i].move_pos.x + g_polygon[i].vertex[j + 1].x, g_polygon[i].move_pos.y + g_polygon[i].vertex[j + 1].y);

				}
				g_vertex.emplace_back(g_polygon[i].move_pos + g_polygon[i].vertex[g_polygon[i].vertex.size() - 1]);
				Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
					, g_polygon[i].move_pos.x + g_polygon[i].vertex[g_polygon[i].vertex.size() - 1].x, g_polygon[i].move_pos.y + g_polygon[i].vertex[g_polygon[i].vertex.size() - 1].y
					, g_polygon[i].move_pos.x + g_polygon[i].vertex[0].x, g_polygon[i].move_pos.y + g_polygon[i].vertex[0].y);
			}
			if (g_vertex.size() > g_polygon[i].vertex.size() + 1) {
				make_polygon(g_polygon[i].info);
				g_polygon.erase(g_polygon.begin() + i);

			}

			cnt = 0;
			g_vertex.clear();
		}




		for (int i = 0; i < g_Triangle.size(); ++i) {
			g_vertex.emplace_back(g_Triangle[i].move_pos + Triangle_vertex[0]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_Triangle[i].move_pos.x + Triangle_vertex[0].x, g_Triangle[i].move_pos.y + Triangle_vertex[0].y
				, g_Triangle[i].move_pos.x + Triangle_vertex[1].x, g_Triangle[i].move_pos.y + Triangle_vertex[1].y);

			g_vertex.emplace_back(g_Triangle[i].move_pos + Triangle_vertex[1]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_Triangle[i].move_pos.x + Triangle_vertex[1].x, g_Triangle[i].move_pos.y + Triangle_vertex[1].y
				, g_Triangle[i].move_pos.x + Triangle_vertex[2].x, g_Triangle[i].move_pos.y + Triangle_vertex[2].y);

			g_vertex.emplace_back(g_Triangle[i].move_pos + Triangle_vertex[2]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_Triangle[i].move_pos.x + Triangle_vertex[2].x, g_Triangle[i].move_pos.y + Triangle_vertex[2].y
				, g_Triangle[i].move_pos.x + Triangle_vertex[0].x, g_Triangle[i].move_pos.y + Triangle_vertex[0].y);



			if (g_vertex.size() > 4) {
				make_polygon(g_Triangle[i].info);
				g_Triangle.erase(g_Triangle.begin() + i);

			}

			cnt = 0;
			g_vertex.clear();
		}


		for (int i = 0; i < g_Rectangle.size(); ++i) {
			g_vertex.emplace_back(g_Rectangle[i].move_pos + Rect_vertex[0]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_Rectangle[i].move_pos.x + Rect_vertex[0].x, g_Rectangle[i].move_pos.y + Rect_vertex[0].y
				, g_Rectangle[i].move_pos.x + Rect_vertex[1].x, g_Rectangle[i].move_pos.y + Rect_vertex[1].y);

			g_vertex.emplace_back(g_Rectangle[i].move_pos + Rect_vertex[1]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_Rectangle[i].move_pos.x + Rect_vertex[1].x, g_Rectangle[i].move_pos.y + Rect_vertex[1].y
				, g_Rectangle[i].move_pos.x + Rect_vertex[2].x, g_Rectangle[i].move_pos.y + Rect_vertex[2].y);

			g_vertex.emplace_back(g_Rectangle[i].move_pos + Rect_vertex[2]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_Rectangle[i].move_pos.x + Rect_vertex[2].x, g_Rectangle[i].move_pos.y + Rect_vertex[2].y
				, g_Rectangle[i].move_pos.x + Rect_vertex[3].x, g_Rectangle[i].move_pos.y + Rect_vertex[3].y);

			g_vertex.emplace_back(g_Rectangle[i].move_pos + Rect_vertex[3]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_Rectangle[i].move_pos.x + Rect_vertex[3].x, g_Rectangle[i].move_pos.y + Rect_vertex[3].y
				, g_Rectangle[i].move_pos.x + Rect_vertex[0].x, g_Rectangle[i].move_pos.y + Rect_vertex[0].y);

			if (g_vertex.size() > 5) {
				make_polygon(g_Rectangle[i].info);
				g_Rectangle.erase(g_Rectangle.begin() + i);

			}

			cnt = 0;
			g_vertex.clear();
		}



		for (int i = 0; i < g_pentagon.size(); ++i) {
			g_vertex.emplace_back(g_pentagon[i].move_pos + Pentagon_vertex[0]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_pentagon[i].move_pos.x + Pentagon_vertex[0].x, g_pentagon[i].move_pos.y + Pentagon_vertex[0].y
				, g_pentagon[i].move_pos.x + Pentagon_vertex[1].x, g_pentagon[i].move_pos.y + Pentagon_vertex[1].y);

			g_vertex.emplace_back(g_pentagon[i].move_pos + Pentagon_vertex[1]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_pentagon[i].move_pos.x + Pentagon_vertex[1].x, g_pentagon[i].move_pos.y + Pentagon_vertex[1].y
				, g_pentagon[i].move_pos.x + Pentagon_vertex[2].x, g_pentagon[i].move_pos.y + Pentagon_vertex[2].y);

			g_vertex.emplace_back(g_pentagon[i].move_pos + Pentagon_vertex[2]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_pentagon[i].move_pos.x + Pentagon_vertex[2].x, g_pentagon[i].move_pos.y + Pentagon_vertex[2].y
				, g_pentagon[i].move_pos.x + Pentagon_vertex[3].x, g_pentagon[i].move_pos.y + Pentagon_vertex[3].y);

			g_vertex.emplace_back(g_pentagon[i].move_pos + Pentagon_vertex[3]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_pentagon[i].move_pos.x + Pentagon_vertex[3].x, g_pentagon[i].move_pos.y + Pentagon_vertex[3].y
				, g_pentagon[i].move_pos.x + Pentagon_vertex[4].x, g_pentagon[i].move_pos.y + Pentagon_vertex[4].y);


			g_vertex.emplace_back(g_pentagon[i].move_pos + Pentagon_vertex[4]);
			Collision_Check(Line_vertex[0].x, Line_vertex[0].y, Line_vertex[1].x, Line_vertex[1].y
				, g_pentagon[i].move_pos.x + Pentagon_vertex[4].x, g_pentagon[i].move_pos.y + Pentagon_vertex[4].y
				, g_pentagon[i].move_pos.x + Pentagon_vertex[0].x, g_pentagon[i].move_pos.y + Pentagon_vertex[0].y);

			if (g_vertex.size() > 6) {
				make_polygon(g_pentagon[i].info);
				g_pentagon.erase(g_pentagon.begin() + i);

			}
			cnt = 0;
			g_vertex.clear();
		}





		Line_vertex[0].x = -2.0f;
		Line_vertex[0].y = -2.0f;
		Line_vertex[1].x = -2.0f;
		Line_vertex[1].y = -2.0f;


	}
}
GLvoid Motion(int x, int y)
{
	auto move_pos = ConvertWinToGL(x, y);
	Line_vertex[1].x = move_pos.first;
	Line_vertex[1].y = move_pos.second;

	glutPostRedisplay();
}

GLvoid Timer(int value)
{
	random_device rd;
	default_random_engine dre(rd());
	uniform_int_distribution<int> x_pos(-200, 0);
	uniform_int_distribution<int> y_pos(200, 400);
	uniform_int_distribution<int> cooltime(1000, 3000);
	switch (value) {
	case 0:
		for (int i = 0; i < g_Rectangle.size(); ++i) {
			g_Rectangle[i].deltaTime += 0.001 * 20 * TIMER;
		}
		for (int i = 0; i < g_pentagon.size(); ++i) {
			g_pentagon[i].deltaTime += 0.001 * 20 * TIMER;
		}
		for (int i = 0; i < g_polygon.size(); ++i) {
			g_polygon[i].deltaTime += 0.001 * 20 * TIMER;
		}
		for (int i = 0; i < g_Triangle.size(); ++i) {
			g_Triangle[i].deltaTime += 0.001 * 20 * TIMER;
		}
		glutPostRedisplay();
		glutTimerFunc(20, Timer, 0);
		break;
	case 1:
		g_Rectangle.emplace_back(x_pos(dre), y_pos(dre));
		glutTimerFunc(cooltime(dre), Timer, 1);
		break;
	case 2:
		g_pentagon.emplace_back(x_pos(dre), y_pos(dre));
		glutTimerFunc(cooltime(dre), Timer, 2);
		break;
	case 3:
		g_Triangle.emplace_back(x_pos(dre), y_pos(dre));
		glutTimerFunc(cooltime(dre), Timer, 3);
		break;
	case 4:
		for (int i = 0; i < g_Rectangle.size(); ++i) {
			g_Rectangle[i].falling();

		}
		for (int i = 0; i < g_pentagon.size(); ++i) {
			g_pentagon[i].falling();

		}
		for (int i = 0; i < g_Triangle.size(); ++i) {
			g_Triangle[i].falling();
		}

		for (int i = 0; i < g_polygon.size(); ++i) {
			g_polygon[i].falling();
		}
		glutTimerFunc(20, Timer, 4);
		break;
	case 5:
		Basket.Move();
		glutTimerFunc(20, Timer, 5);
		break;
	case 6:
		for (int i = 0; i < g_polygon.size(); ++i) {
			for (int j = 0; j < g_polygon[i].vertex.size(); ++j) {
				if ((abs((g_polygon[i].vertex[j].x + g_polygon[i].move_pos.x) - Basket.move_pos.x) < 0.25f) &&
					(g_polygon[i].vertex[j].y + g_polygon[i].move_pos.y) < -0.79f)
				{
					g_polygon[i].fall = FALSE;
					g_polygon[i].vertex[j].x = Basket.move_pos.x - g_polygon[i].move_pos.x;
					if (!Basket.toggle_move)
					{
						g_polygon[i].move_pos.x += 0.01f;
					}
					else
					{
						g_polygon[i].move_pos.x -= 0.01f;
					}
					
					break;
					
				}

			}
		}
		glutTimerFunc(20, Timer, 6);
		break;

	}


}

GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{

	switch (key)
	{

	case 's':
		button_s = !button_s;
		break;
	case 'l':
		button_l = !button_l;
		break;
	case '=':
	case '+':
		if (TIMER < 2.0) {
			TIMER += 0.1;
		}

		break;
	case '-':

		if (TIMER > 0.1) {
			TIMER -= 0.1;
		}
		break;


	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();

}
void Collision_Check(float x, float y, float x1, float y1, float polygon_x1, float polygon_y1, float polygon_x2, float polygon_y2) {
	float angle1 = 0, constant1 = 0, fsamevalue1 = 0, Fsamevalue1 = 0;
	float angle2 = 0, constant2 = 0, fsamevalue2 = 0, Fsamevalue2 = 0;
	float max_x = 0, max_y = 0;
	float min_x = 0, min_y = 0;
	float max1_x = 0, max1_y = 0;
	float min1_x = 0, min1_y = 0;

	if (x >= x1) {
		max1_x = x;
		min1_x = x1;
	}
	else {
		min1_x = x;
		max1_x = x1;
	}
	if (y >= y1) {
		max1_y = y;
		min1_y = y1;
	}
	else {
		min1_y = y;
		max1_y = y1;
	}

	if (polygon_x1 >= polygon_x2) {
		max_x = polygon_x1;
		min_x = polygon_x2;
	}
	else {
		min_x = polygon_x1;
		max_x = polygon_x2;
	}
	if (polygon_y1 >= polygon_y2) {
		max_y = polygon_y1;
		min_y = polygon_y2;
	}
	else {
		min_y = polygon_y1;
		max_y = polygon_y2;
	}

	if (x == x1) {
		fsamevalue1 = x;
	}
	else {
		angle1 = (float)(y1 - y) / (x1 - x);
		constant1 = y - angle1 * x;
	}

	if (polygon_x1 == polygon_x2) {
		fsamevalue2 = polygon_x1;
	}
	else {
		angle2 = (float)(polygon_y2 - polygon_y1) / (polygon_x2 - polygon_x1);
		constant2 = polygon_y1 - angle2 * polygon_x1;
	}

	float result_x = 0, result_y = 0;
	if (x == x1 && polygon_x1 == polygon_x2) { return; }
	if (x == x1) {
		result_x = fsamevalue1;
		result_y = angle2 * fsamevalue1 + constant2;
	}
	else if (polygon_x1 == polygon_x2) {
		result_x = fsamevalue2;
		result_y = angle1 * fsamevalue2 + constant1;
	}
	else {
		result_x = -(constant1 - constant2) / (angle1 - angle2);
		result_y = angle1 * result_x + constant1;
	}

	if (min1_x <= result_x && result_x <= max1_x) {
		if (min1_y <= result_y && result_y <= max1_y) {
			if (min_x < result_x && result_x < max_x) {
				if (min_y < result_y && result_y < max_y) {
					g_vertex.emplace_back(result_x, result_y, 0);
					point[cnt] = g_vertex.size() - 1;
					cnt++;
				}
				else if (abs(result_y - min_y) < MIN_DIFF_FLT || abs(result_y - max_y) < MIN_DIFF_FLT) {
					g_vertex.emplace_back(result_x, result_y, 0);
					point[cnt] = g_vertex.size() - 1;
					cnt++;
				}
			}
			else if (abs(result_x - min_x) < MIN_DIFF_FLT || abs(result_x - max_x) < MIN_DIFF_FLT) {
				if (min_y < result_y && result_y < max_y) {
					g_vertex.emplace_back(result_x, result_y, 0);
					point[cnt] = g_vertex.size() - 1;
					cnt++;
				}
				else if (abs(result_y - min_y) < MIN_DIFF_FLT || abs(result_y - max_y) < MIN_DIFF_FLT) {
					g_vertex.emplace_back(result_x, result_y, 0);
					point[cnt] = g_vertex.size() - 1;
					cnt++;
				}
			}
		}
	}
}

void make_polygon(Information information)
{
	information.x_speed *= 0.3;
	vector <vec3> temp;
	for (int i = 0; i < g_vertex.size(); ++i) {

		temp.emplace_back(g_vertex[i]);
		if (point[0] == i)
		{
			i = point[1] - 1;
		}

	}

	g_polygon.emplace_back(temp);
	g_polygon.back().info = information;
	temp.clear();
	for (int i = point[0]; i <= point[1]; ++i)
	{
		temp.emplace_back(g_vertex[i]);
	}

	g_polygon.emplace_back(temp);
	g_polygon.back().info = information;
	g_polygon.back().info.x_speed *= -1;
	temp.clear();

}
void make_vertexShaders() {
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수

	//vertex shader read, save vertexSource
	vertexSource = filetobuf("vertex_hw.glsl");

	//vertex shader 생성 해주고 생성한 데이터를 vertexShader라는 변수에 저장
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//성공하면 0이 아닌값

	//세이더 객체, 1, 세이더 소스 코드, NULL
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//shader, 문자열들의 갯수, 문자열들의 배열, 문자열들의 길이를 저장할 int배열

	//컴파일 해야지요.
	glCompileShader(vertexShader);

	//세이더 결과 어떻게 됐는지? 상태 확인하는 코드
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result) {//result == 0이면 오류가 있다.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}
void make_fragmentShaders() {
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
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

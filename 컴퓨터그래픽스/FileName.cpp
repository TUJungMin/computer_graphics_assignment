#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vector>

#pragma warning(disable:4996)

using namespace std;

GLvoid drawScene();
GLvoid Reshape(int w, int h);
void InitBuffer();
void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
char* filetobuf(const char* file);

enum
{
    Point,
    Line,
    Tri,
    Rect
};

#define winWidth 800
#define winHeight 800
#define MAX_NUM 10

GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vao[10], vbo[2];
int shape_flag = 0;
int num = -1;

int l_num = 0;
int t_num = 0;
int r_num = 0;
int p_num = 0;
int v_num = 0;

int draw_num = 0;
int shape_num[10] = {};

GLfloat PointShape[10][1][3] = {}; //--- 삼각형 위치 값
GLfloat TriShape[10][3][3] = {}; //--- 삼각형 위치 값
GLfloat LineShape[10][2][3] = {}; //--- 삼각형 위치 값
GLfloat RectShape[10][4][3] = {}; //--- 삼각형 위치 값
GLfloat PointColor[1][3] = { {0,0,1} }; //--- 삼각형 위치 값
GLfloat TriColor[1][3] = { {1,0,0} }; //--- 삼각형 위치 값
GLfloat LineColor[1][3] = { {0,0,0} }; //--- 삼각형 위치 값
GLfloat RectColor[1][3] = { {1,0,0} }; //--- 삼각형 위치 값

GLfloat Change_GLcoord(int n, int lenth) // x면 그대로, y면 함수 앞에 - 붙이기
{
    GLfloat coord = (GLfloat)(n - lenth / 2) * (GLfloat)(1.0 / (GLfloat)(lenth / 2.0));
    return coord;
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'p':
    case 'P':
        shape_flag = Point; // 점
        break;
    case 'l':
    case 'L':
        shape_flag = Line; // 선
        break;
    case 't':
    case 'T':
        shape_flag = Tri; // 삼각형
        break;
    case 'r':
    case 'R':
        shape_flag = Rect; // 삼각형 두개
        break;
    case 'w':
    case 'W':
        break;
    case 'S':
    case 's':
        break;
    case 'd':
    case 'D':
        break;
    case 'a':
    case 'A':
        break;
    case 'c':
    case 'C':
        num = 0;
        break;

    }
    glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

void Mouse(int button, int state, int x, int y)
{
    GLfloat _x = Change_GLcoord(x, winWidth);
    GLfloat _y = -Change_GLcoord(y, winHeight);

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            if (num != 9)
            {
                switch (shape_flag)
                {
                case Point:
                    num++;
                    shape_num[num] = Point;

                    PointShape[p_num][0][0] = _x;
                    PointShape[p_num++][0][1] = _y;
                    break;
                case Tri:
                    num++;
                    shape_num[num] = Tri;

                    TriShape[t_num][0][0] = _x;
                    TriShape[t_num++][0][1] = _y + 0.05;

                    TriShape[t_num][1][0] = _x - 0.03;
                    TriShape[t_num++][1][1] = _y - 0.02;

                    TriShape[t_num][2][0] = _x + 0.03;
                    TriShape[t_num++][2][1] = _y - 0.02;
                    break;
                case Line:
                    num++;
                    shape_num[num] = Line;

                    LineShape[l_num][0][0] = _x;
                    LineShape[l_num++][0][1] = _y;

                    LineShape[l_num][1][0] = _x + 0.1;
                    LineShape[l_num++][1][1] = _y + 0.1;
                    break;
                case Rect:
                    num++;
                    shape_num[num] = Rect;

                    RectShape[r_num][0][0] = _x - 0.05;
                    RectShape[r_num++][0][1] = _y + 0.05;

                    RectShape[r_num][1][0] = _x - 0.05;
                    RectShape[r_num++][1][1] = _y - 0.05;

                    RectShape[r_num][2][0] = _x + 0.05;
                    RectShape[r_num++][2][1] = _y - 0.05;

                    RectShape[r_num][3][0] = _x + 0.05;
                    RectShape[r_num++][3][1] = _y + 0.05;
                    break;
                }
            }
        }
    }
}

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
    //--- 윈도우 생성하기
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Example1");
    //--- GLEW 초기화하기
    glewExperimental = GL_TRUE;
    glewInit();
    make_shaderProgram();
    InitBuffer();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMainLoop();
}

GLvoid drawScene()
{
    draw_num = 0;
    //--- 변경된 배경색 설정
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    //glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //--- 렌더링 파이프라인에 세이더 불러오기
    glUseProgram(shaderProgramID);
    //--- 사용할 VAO 불러오기
    for (int i = 0; i < num; i++)
    {
        glBindVertexArray(vao[i]);
        switch (shape_num[i])
        {
        case Point:
            glDrawArrays(GL_POINT, draw_num, 1);
            draw_num++;
            break;
        case Tri:
            glDrawArrays(GL_TRIANGLES, draw_num, 3);
            draw_num += 3;
            break;
        case Line:
            glDrawArrays(GL_LINE, draw_num, 2);
            draw_num += 2;
            break;
        case Rect:
            glDrawArrays(GL_TRIANGLE_STRIP, draw_num, 4);
            draw_num += 4;
            break;
        }

    }
    //--- 삼각형 그리기
    glutSwapBuffers(); //--- 화면에 출력하기
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void InitBuffer()
{
    v_num = 0;
    for (int i = v_num; i < v_num + p_num; i++)
    {
        glGenVertexArrays(1, &vao[i]); //--- VAO 를 지정하고 할당하기
        glBindVertexArray(vao[i]); //--- VAO를 바인드하기
        glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
        //--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        //--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
        //--- triShape 배열의 사이즈: 40 * float
        glBufferData(GL_ARRAY_BUFFER, p_num * 3 * sizeof(GLfloat), PointShape, GL_STATIC_DRAW);
        //--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute 인덱스 0번을 사용가능하게 함
        glEnableVertexAttribArray(0);
        //--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        //--- 변수 colors에서 버텍스 색상을 복사한다.
        //--- colors 배열의 사이즈: 40 *float 
        glBufferData(GL_ARRAY_BUFFER, p_num * 3 * sizeof(GLfloat), PointColor, GL_STATIC_DRAW);
        //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute 인덱스 1번을 사용 가능하게 함.
        glEnableVertexAttribArray(1);
    }
    v_num += p_num;
    for (int i = v_num; i < v_num + l_num; i++)
    {
        glGenVertexArrays(1, &vao[i]); //--- VAO 를 지정하고 할당하기
        glBindVertexArray(vao[i]); //--- VAO를 바인드하기
        glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
        //--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        //--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
        //--- triShape 배열의 사이즈: 40 * float
        glBufferData(GL_ARRAY_BUFFER, l_num * 3 * 2 * sizeof(GLfloat), LineShape, GL_STATIC_DRAW);
        //--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute 인덱스 0번을 사용가능하게 함
        glEnableVertexAttribArray(0);
        //--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        //--- 변수 colors에서 버텍스 색상을 복사한다.
        //--- colors 배열의 사이즈: 40 *float 
        glBufferData(GL_ARRAY_BUFFER, l_num * 3 * 2 * sizeof(GLfloat), LineColor, GL_STATIC_DRAW);
        //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute 인덱스 1번을 사용 가능하게 함.
        glEnableVertexAttribArray(1);
    }
    v_num += l_num;
    for (int i = v_num; i < v_num + r_num; i++)
    {
        glGenVertexArrays(1, &vao[i]); //--- VAO 를 지정하고 할당하기
        glBindVertexArray(vao[i]); //--- VAO를 바인드하기
        glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
        //--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        //--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
        //--- triShape 배열의 사이즈: 40 * float
        glBufferData(GL_ARRAY_BUFFER, r_num * 3 * 4 * sizeof(GLfloat), RectShape, GL_STATIC_DRAW);
        //--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute 인덱스 0번을 사용가능하게 함
        glEnableVertexAttribArray(0);
        //--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        //--- 변수 colors에서 버텍스 색상을 복사한다.
        //--- colors 배열의 사이즈: 40 *float 
        glBufferData(GL_ARRAY_BUFFER, r_num * 3 * 4 * sizeof(GLfloat), RectColor, GL_STATIC_DRAW);
        //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute 인덱스 1번을 사용 가능하게 함.
        glEnableVertexAttribArray(1);
    }
    v_num += r_num;
    for (int i = v_num; i < v_num + t_num; i++)
    {
        glGenVertexArrays(1, &vao[i]); //--- VAO 를 지정하고 할당하기
        glBindVertexArray(vao[i]); //--- VAO를 바인드하기
        glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
        //--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        //--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
        //--- triShape 배열의 사이즈: 40 * float
        glBufferData(GL_ARRAY_BUFFER, t_num * 3 * 3 * sizeof(GLfloat), TriShape, GL_STATIC_DRAW);
        //--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute 인덱스 0번을 사용가능하게 함
        glEnableVertexAttribArray(0);
        //--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        //--- 변수 colors에서 버텍스 색상을 복사한다.
        //--- colors 배열의 사이즈: 40 *float 
        glBufferData(GL_ARRAY_BUFFER, t_num * 3 * 3 * sizeof(GLfloat), TriColor, GL_STATIC_DRAW);
        //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute 인덱스 1번을 사용 가능하게 함.
        glEnableVertexAttribArray(1);
    }

}

void make_shaderProgram()
{
    make_vertexShaders(); //--- 버텍스 세이더 만들기
    make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
    //-- shader Program
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);
    //--- 세이더 삭제하기
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //--- Shader Program 사용하기
    glUseProgram(shaderProgramID);
}

void make_vertexShaders()
{
    vertexSource = filetobuf("vertex.glsl");
    //--- 버텍스 세이더 객체 만들기
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //--- 세이더 코드를 세이더 객체에 넣기
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    //--- 버텍스 세이더 컴파일하기
    glCompileShader(vertexShader);
    //--- 컴파일이 제대로 되지 않은 경우: 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
        return;
    }
}

void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragment.glsl");
    //--- 프래그먼트 세이더 객체 만들기
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //--- 세이더 코드를 세이더 객체에 넣기
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    //--- 프래그먼트 세이더 컴파일
    glCompileShader(fragmentShader);
    //--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << endl;
        return;
    }
}

char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb"); // Open file for reading
    if (!fptr) // Return NULL on failure
    {
        cout << file << "파일 못찾음" << endl;
        exit(1);
    }
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file
    length = ftell(fptr); // Find out how many bytes into the file we are
    buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
    fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
    fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
    fclose(fptr); // Close the file
    buf[length] = 0; // Null terminator
    return buf; // Return the buffer
}

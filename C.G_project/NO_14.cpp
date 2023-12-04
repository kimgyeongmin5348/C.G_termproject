#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define MAXX 1000
#define MAXY 600

const GLfloat shape1[4][3] = {
	{0.5,0.5,0.5},{-0.5,0.5,0.5},{-0.5,-0.5,0.5},{0.5,-0.5,0.5}
};

const GLfloat shape2[4][3] = {
	{0.5,0.5,-0.5},{-0.5,0.5,-0.5},{-0.5,-0.5,-0.5},{0.5,-0.5,-0.5}
};

const GLfloat shape3[4][3] = {
	{0.5,0.5,0.5},{-0.5,0.5,0.5},{-0.5,0.5,-0.5},{0.5,0.5,-0.5}
};

const GLfloat shape4[4][3] = {
	{0.5,-0.5,0.5},{-0.5,-0.5,0.5},{-0.5,-0.5,-0.5},{0.5,0.5,-0.5}
};

const GLfloat shape5[4][3] = {
	{-0.5,0.5,0.5},{-0.5,0.5,-0.5},{-0.5,-0.5,-0.5},{-0.5,-0.5,0.5}
};

const GLfloat shape6[4][3] = {
	{0.5,0.5,0.5},{0.5,0.5,-0.5},{0.5,-0.5,-0.5},{0.5,-0.5,0.5}
};

const GLfloat colors[6][3] = {
	{1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 0.0, 1.0},
	{1.0, 1.0, 0.0},
	{1.0, 0.0, 1.0},
	{0.0, 1.0, 1.0}
};


void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);


GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao, vbo[2];

int shape_count;
int shape_type;


void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{

	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(MAXX, MAXY);
	glutCreateWindow("C.G_NO.14");
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = 1.0;
	gColor = 1.0;
	bColor = 0.4;
	glClearColor(0.0, 0.0, 0.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	glm::mat4 Rx = glm::mat4(1.0f); //--- 이동 행렬 선언
	glm::mat4 Ry = glm::mat4(1.0f); //--- 회전 행렬 선언
	glm::mat4 RR = glm::mat4(1.0f);

	Rx = glm::rotate(Rx, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
	Ry = glm::rotate(Ry, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));

	RR = Rx * Ry;

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);

	switch (shape_type) {
	case 1:
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glUniformMatrix4fv(PosLocation, 1, GL_FALSE, glm::value_ptr(RR));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		break;
	case 2:
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glUniformMatrix4fv(PosLocation, 1, GL_FALSE, glm::value_ptr(RR));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		break;
	case 3:
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glUniformMatrix4fv(PosLocation, 1, GL_FALSE, glm::value_ptr(RR));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		break;
	case 4:
		glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
		glUniformMatrix4fv(PosLocation, 1, GL_FALSE, glm::value_ptr(RR));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		break;
	case 5:
		glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
		glUniformMatrix4fv(PosLocation, 1, GL_FALSE, glm::value_ptr(RR));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		break;
	case 6:
		glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
		glUniformMatrix4fv(PosLocation, 1, GL_FALSE, glm::value_ptr(RR));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		break;



	}
	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(ColorLocation);


	// x축 그리기
	glBegin(GL_LINES);
	glColor3f(rColor, 0.0, bColor);
	glVertex3f(-1.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glEnd();
	// y축 그리기
	glBegin(GL_LINES);
	glColor3f(0.0, gColor, bColor);
	glVertex3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glEnd();

	glutSwapBuffers(); // 화면에 출력하기


}

void InitBuffer() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(6, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), shape1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), shape2, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), shape3, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), shape4, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), shape5, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), shape6, GL_STATIC_DRAW);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(5);

}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
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
		std::cout << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
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
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
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

char* filetobuf(const char* file)
{
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

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 1:
		shape_type = 1;
		break;
	case 2:
		shape_type = 2;
		break;
	case 3:
		shape_type = 3;
		break;
	case 4:
		shape_type = 4;
		break;
	case 5:
		shape_type = 5;
		break;
	case 6:
		shape_type = 6;
		break;
	}
	glutPostRedisplay();
}
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
#include <random>

#define width 1000
#define height 800
#define h_vertex 0.2f
#define q_vertex 0.3f

using namespace std;

random_device rd;
mt19937 gen(rd());

float hexa[] =
{   //육면체
	//윗면
	-h_vertex, h_vertex, h_vertex,
	h_vertex, h_vertex, h_vertex,
	-h_vertex, h_vertex, -h_vertex,

	h_vertex, h_vertex, h_vertex,
	h_vertex, h_vertex, -h_vertex,
	-h_vertex, h_vertex, -h_vertex,
	//정면
	-h_vertex, -h_vertex, h_vertex,
	h_vertex, -h_vertex, h_vertex,
	-h_vertex, h_vertex, h_vertex,

	h_vertex, -h_vertex, h_vertex,
	h_vertex, h_vertex, h_vertex,
	-h_vertex, h_vertex, h_vertex,

	//아랫면
	-h_vertex, -h_vertex, h_vertex,
	-h_vertex, -h_vertex, -h_vertex,
	h_vertex, -h_vertex, -h_vertex,

	-h_vertex, -h_vertex, h_vertex,
	h_vertex, -h_vertex, -h_vertex,
	h_vertex, -h_vertex, h_vertex,

	//뒷면
	h_vertex, -h_vertex, -h_vertex,
	-h_vertex, -h_vertex, -h_vertex,
	h_vertex, h_vertex, -h_vertex,

	-h_vertex, -h_vertex, -h_vertex,
	-h_vertex, h_vertex, -h_vertex,
	h_vertex, h_vertex, -h_vertex,


	//좌측면
	-h_vertex, -h_vertex, -h_vertex,
	-h_vertex, h_vertex, h_vertex,
	-h_vertex, h_vertex, -h_vertex,

	-h_vertex, -h_vertex, -h_vertex,
	-h_vertex, -h_vertex, h_vertex,
	-h_vertex, h_vertex, h_vertex,

	//우측면
   h_vertex, -h_vertex, h_vertex,
   h_vertex, -h_vertex, -h_vertex,
   h_vertex, h_vertex, h_vertex,

   h_vertex, -h_vertex, -h_vertex,
   h_vertex, h_vertex, -h_vertex,
   h_vertex, h_vertex, h_vertex
};

float bottom[] =
{
	1, 0, -1,
	-1, 0, 1,
	1, 0, 1,
	1, 0, -1,
	-1, 0, -1,
	-1, 0, 1
};

float orignalSpeed = 0.01f;
float orignalRadian = 40;
float baseSpeed = 0.01f;
float baseRadian = 40;

GLvoid drawScene();
GLvoid Reshape(int, int);
GLvoid KeyBoard(unsigned char, int, int);
GLvoid KeyBoardUp(unsigned char, int, int);
GLvoid view(int);
GLvoid Setting();
GLint Collision(float, float, float, float);
bool DoCollision(float, float, float);

GLUquadricObj* qobj;
GLuint s_program;
GLuint VAO[3], VBO[6];
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;
GLenum obj_type{ GL_FILL };
GLint g_window_w, g_window_h;

typedef struct object {
	float x_rotate{}, y_rotate{}, z_rotate{};
	float x_rotate_aoc{}, y_rotate_aoc{}, z_rotate_aoc{};

	float x_trans{}, y_trans{}, z_trans{}, x_trans1{};
	float x_trans_aoc{}, y_trans_aoc{}, z_trans_aoc{};
	float x_scale{}, y_scale{}, z_scale{};

	bool rotate_up{ true };
	bool is_jump{ false };
	bool y_trans_up{ true };
}O;

typedef struct function {
	bool cull{ false };
	bool y_rotate{ false };
	bool x_is_trans{ false }, y_is_trans{ false }, z_is_trans{ false };
	bool x_is_reverse_trans{ false }, y_is_reverse_trans{ false }, z_is_reverse_trans{ false };
	bool x_is_rotate{ false }, y_is_rotate{ false }, z_is_rotate{ false };
	bool x_is_reverse_rotate{ false }, y_is_reverse_rotate{ false }, z_is_reverse_rotate{ false };
	bool start_timer{ true };
	bool door_open{ false };
	bool is_robot_move{ false };
	bool robot_front_move{ false };
	bool robot_back_move{ false };
	bool robot_left_move{ false };
	bool robot_right_move{ false };
	bool Collision{ false };
	bool SpeedUp{ false };
	bool SpeedDown{ false };
	bool robot_see{ false };
}F;

O box, temp, camera, robot_arm, robot_leg, robot, block, block2, block3;
O l;
F h_f, temp_f;
int count{};
bool change_type{ false };
bool pers{ true };


GLchar* filetobuf(const GLchar* file)
{
	FILE* fptr;
	long length;
	GLchar* buf;

	fopen_s(&fptr, file, "rb");
	if (!fptr)
		return NULL;

	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (GLchar*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;

	return buf;
}

void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
}

void make_fragmentShader()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

}

void InitShader()
{
	make_vertexShaders();
	make_fragmentShader();

	s_program = glCreateProgram();

	glAttachShader(s_program, vertexShader);
	glAttachShader(s_program, fragmentShader);
	glLinkProgram(s_program);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR :  fragment Shader Fail Compile \n" << errorLog << endl;
		exit(-1);
	}

	else
		cout << "good";
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(s_program);
}

GLvoid InitBuffer()
{
	glGenVertexArrays(3, VAO);

	//->line
	glBindVertexArray(VAO[0]);
	glGenBuffers(2, &VBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hexa), hexa, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO[1]);
	glGenBuffers(2, &VBO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bottom), bottom, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void drawScene()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //깊이 체크 (컬링)
	glUseProgram(s_program);

	glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 1.0f);			//위치
	glm::vec3 cameraDirection = glm::vec3(0.0f, 1.0f, 0.0f);	//바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);			//카메라 상향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	view = glm::rotate(view, glm::radians(10.f + camera.x_rotate_aoc), glm::vec3(1.0f, 0.0f, 0.0f));

	view = glm::rotate(view, glm::radians(camera.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));

	view = glm::translate(view, glm::vec3(camera.x_trans, camera.y_trans, camera.z_trans));


	unsigned int viewLocation = glGetUniformLocation(s_program, "view");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	if (!pers) {
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -100.0f, 100.0f);
		unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	else {
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
		unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}

	//무대 윗면
	glm::mat4 Box = glm::mat4(1.0f);
	Box = glm::translate(Box, glm::vec3(0.f, 1, 0.f));
	Box = glm::scale(Box, glm::vec3(5.0f, 5.0f, 5.0f));
	unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Box));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	int objColorLocation = glGetUniformLocation(s_program, "objectColor");
	unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 1.0f, 1.0f, 1.0f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//앞면(오른쪽)
	Box = glm::mat4(1.0f);
	Box = glm::translate(Box, glm::vec3(0.5f, 0.f, 0.f));
	Box = glm::translate(Box, glm::vec3(box.x_trans, 0.f+1, 0.f));
	Box = glm::scale(Box, glm::vec3(2.5f, 5.0f, 5.0f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Box));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.3f, 0.7f, 0.3f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 6, 6);
	//앞면(왼쪽)
	Box = glm::mat4(1.0f);
	Box = glm::translate(Box, glm::vec3(-0.5f, 0.f, 0.f));
	Box = glm::translate(Box, glm::vec3(box.x_trans1, 0.f + 1, 0.f));
	Box = glm::scale(Box, glm::vec3(2.5f, 5.0f, 5.0f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Box));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.3f, 0.7f, 0.3f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 6, 6);
	//밑면
	Box = glm::mat4(1.0f);
	Box = glm::translate(Box, glm::vec3(0.f, 1, 0.f));
	Box = glm::scale(Box, glm::vec3(5.0f, 5.0f, 5.0f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Box));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.3f, 0.5f, 0.8f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 12, 6);
	//뒷면
	Box = glm::mat4(1.0f);
	Box = glm::translate(Box, glm::vec3(0.f, 1, 0.f));
	Box = glm::scale(Box, glm::vec3(5.0f, 5.0f, 5.0f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Box));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.4f, 0.3f, 0.5f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 18, 6);
	//옆면
	Box = glm::mat4(1.0f);
	Box = glm::translate(Box, glm::vec3(0.f, 1, 0.f));
	Box = glm::scale(Box, glm::vec3(5.0f, 5.0f, 5.0f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Box));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.3f, 0.4f, 0.3f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 24, 12);

	//=====================================================================================================================================
	
	//장애물1
	glm::mat4 Block = glm::mat4(1.0f);
	Block = glm::translate(Block, glm::vec3(block.x_trans, 0.0f, block.z_trans));
	Block = glm::scale(Block, glm::vec3(1.0f, 0.4f, 1.0f));
	Block = glm::translate(Block, glm::vec3(0.f, 0.2f, 0.f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Block));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 1.0f, 1.0f, 0.0f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//장애물2
	glm::mat4 Block2 = glm::mat4(1.0f);
	Block2 = glm::translate(Block2, glm::vec3(block2.x_trans, 0.0f, block2.z_trans));
	Block2 = glm::scale(Block2, glm::vec3(1.0f, 0.4f, 1.0f));
	Block2 = glm::translate(Block2, glm::vec3(0.f, 0.2f, 0.f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Block2));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 1.0f, 1.0f, 0.0f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//장애물3
	glm::mat4 Block3 = glm::mat4(1.0f);
	Block3 = glm::translate(Block3, glm::vec3(block3.x_trans, 0.0f, block3.z_trans));
	Block3 = glm::scale(Block3, glm::vec3(1.0f, 0.4f, 1.0f));
	Block3 = glm::translate(Block3, glm::vec3(0.f, 0.2f, 0.f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Block3));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 1.0f, 1.0f, 0.0f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//===============================================================================================================================================

	//다리1
	glm::mat4 Robot = glm::mat4(1.0f);

	Robot = glm::translate(Robot, glm::vec3(robot.x_trans_aoc, robot.y_trans_aoc, robot.z_trans_aoc));

	Robot = glm::rotate(Robot, glm::radians(robot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

	Robot = glm::translate(Robot, glm::vec3(0.05f, 0.f, 0.f));
	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex, 0.f));
	Robot = glm::rotate(Robot, glm::radians(robot_leg.x_rotate_aoc), glm::vec3(1.0f, 0.0f, 0.0f));
	Robot = glm::translate(Robot, glm::vec3(0.f, -h_vertex, 0.f));
	Robot = glm::translate(Robot, glm::vec3(-0.05f, 0.f, 0.f));

	Robot = glm::translate(Robot, glm::vec3(0.05f, 0.f, 0.f));
	Robot = glm::scale(Robot, glm::vec3(0.2f, 0.5f, 0.2f));
	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex, 0.f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Robot));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.1f, 0.3f, 0.3f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	//다리2
	Robot = glm::mat4(1.0f);

	Robot = glm::translate(Robot, glm::vec3(robot.x_trans_aoc, robot.y_trans_aoc, robot.z_trans_aoc));

	Robot = glm::rotate(Robot, glm::radians(robot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

	Robot = glm::translate(Robot, glm::vec3(0.05f, 0.f, 0.f));
	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex, 0.f));
	Robot = glm::rotate(Robot, glm::radians(-robot_leg.x_rotate_aoc), glm::vec3(1.0f, 0.0f, 0.0f));
	Robot = glm::translate(Robot, glm::vec3(0.f, -h_vertex, 0.f));
	Robot = glm::translate(Robot, glm::vec3(-0.05f, 0.f, 0.f));

	Robot = glm::translate(Robot, glm::vec3(-0.05f, 0.f, 0.f));
	Robot = glm::scale(Robot, glm::vec3(0.2f, 0.5f, 0.2f));
	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex, 0.f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Robot));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.3f, 0.1f, 0.1f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//몸통
	Robot = glm::mat4(1.0f);

	Robot = glm::translate(Robot, glm::vec3(robot.x_trans_aoc, robot.y_trans_aoc, robot.z_trans_aoc));

	Robot = glm::rotate(Robot, glm::radians(robot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex + h_vertex * 0.5, 0.f));
	Robot = glm::scale(Robot, glm::vec3(0.5f, 0.5f, 0.3f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Robot));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.4f, 0.6f, 0.6f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//팔1
	Robot = glm::mat4(1.0f);

	Robot = glm::translate(Robot, glm::vec3(robot.x_trans_aoc, robot.y_trans_aoc, robot.z_trans_aoc));

	Robot = glm::rotate(Robot, glm::radians(robot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

	Robot = glm::translate(Robot, glm::vec3(0.15f, h_vertex, 0.f));
	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex, 0.f));
	Robot = glm::rotate(Robot, glm::radians(-robot_leg.x_rotate_aoc), glm::vec3(1.0f, 0.0f, 0.0f));
	Robot = glm::translate(Robot, glm::vec3(0.f, -h_vertex, 0.f));
	Robot = glm::translate(Robot, glm::vec3(-0.15f, -h_vertex, 0.f));

	Robot = glm::translate(Robot, glm::vec3(0.15f, h_vertex, 0.f));
	Robot = glm::rotate(Robot, glm::radians(10.f), glm::vec3(0.0f, 0.0f, 1.0f));
	Robot = glm::scale(Robot, glm::vec3(0.15f, 0.5f, 0.15f));
	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex, 0.f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Robot));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.6f, 0.1f, 0.3f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//팔2
	Robot = glm::mat4(1.0f);

	Robot = glm::translate(Robot, glm::vec3(robot.x_trans_aoc, robot.y_trans_aoc, robot.z_trans_aoc));

	Robot = glm::rotate(Robot, glm::radians(robot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

	Robot = glm::translate(Robot, glm::vec3(-0.15f, h_vertex, 0.f));
	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex, 0.f));
	Robot = glm::rotate(Robot, glm::radians(robot_leg.x_rotate_aoc), glm::vec3(1.0f, 0.0f, 0.0f));
	Robot = glm::translate(Robot, glm::vec3(0.f, -h_vertex, 0.f));
	Robot = glm::translate(Robot, glm::vec3(0.15f, -h_vertex, 0.f));

	Robot = glm::translate(Robot, glm::vec3(-0.15f, h_vertex, 0.f));
	Robot = glm::rotate(Robot, glm::radians(-10.f), glm::vec3(0.0f, 0.0f, 1.0f));
	Robot = glm::scale(Robot, glm::vec3(0.15f, 0.5f, 0.15f));
	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex, 0.f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Robot));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.1f, 0.2f, 0.3f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//머리
	Robot = glm::mat4(1.0f);

	Robot = glm::translate(Robot, glm::vec3(robot.x_trans_aoc, robot.y_trans_aoc, robot.z_trans_aoc));

	Robot = glm::rotate(Robot, glm::radians(robot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex + h_vertex + h_vertex * 0.4, 0.f));
	Robot = glm::scale(Robot, glm::vec3(0.5f, 0.4f, 0.5f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Robot));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.4f, 0.8f, 0.8f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//코
	Robot = glm::mat4(1.0f);

	Robot = glm::translate(Robot, glm::vec3(robot.x_trans_aoc, robot.y_trans_aoc, robot.z_trans_aoc));

	Robot = glm::rotate(Robot, glm::radians(robot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex + h_vertex + h_vertex * 0.5, 0.08f));
	Robot = glm::scale(Robot, glm::vec3(0.08f, 0.08f, 0.25f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Robot));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.2f, 0.2f, 0.2f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//입
	Robot = glm::mat4(1.0f);

	Robot = glm::translate(Robot, glm::vec3(robot.x_trans_aoc, robot.y_trans_aoc, robot.z_trans_aoc));

	Robot = glm::rotate(Robot, glm::radians(robot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

	Robot = glm::translate(Robot, glm::vec3(0.f, h_vertex + h_vertex + h_vertex * 0.19, 0.05f));
	Robot = glm::scale(Robot, glm::vec3(0.2f, 0.1f, 0.3f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Robot));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 1.0f, 0.2f, 0.2f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//눈 오른쪽
	Robot = glm::mat4(1.0f);

	Robot = glm::translate(Robot, glm::vec3(robot.x_trans_aoc, robot.y_trans_aoc, robot.z_trans_aoc));

	Robot = glm::rotate(Robot, glm::radians(robot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

	Robot = glm::translate(Robot, glm::vec3(0.055f, h_vertex + h_vertex + h_vertex * 0.65, 0.05f));
	Robot = glm::scale(Robot, glm::vec3(0.1f, 0.1f, 0.3f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Robot));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.f, 0.f, 0.f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//눈 왼쪽
	Robot = glm::mat4(1.0f);

	Robot = glm::translate(Robot, glm::vec3(robot.x_trans_aoc, robot.y_trans_aoc, robot.z_trans_aoc));

	Robot = glm::rotate(Robot, glm::radians(robot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

	Robot = glm::translate(Robot, glm::vec3(-0.055f, h_vertex + h_vertex + h_vertex * 0.65, 0.05f));
	Robot = glm::scale(Robot, glm::vec3(0.1f, 0.1f, 0.3f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Robot));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.f, 0.f, 0.f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	glutSwapBuffers();
}
void Reshape(int w, int h) {

	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);

}

GLvoid KeyBoardUp(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w':
		h_f.robot_front_move = false;
		break;
	case 'a':
		h_f.robot_left_move = false;
		break;
	case 's':
		h_f.robot_back_move = false;
		break;
	case 'd':
		h_f.robot_right_move = false;
		break;
	}
	glutPostRedisplay();
}

GLvoid KeyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'o':
		h_f.door_open = !h_f.door_open;
		break;
	case 'y':
		h_f.y_is_rotate = !h_f.y_is_rotate;
		break;
	case 'Y':
		h_f.y_is_reverse_rotate = !h_f.y_is_reverse_rotate;
		break;
	case '+':
		baseSpeed = baseSpeed * 2;
		baseRadian = baseRadian + 20;
		break;
	case '-':
		baseSpeed = baseSpeed / 2;
		baseRadian = baseRadian - 20;
		break;
	case 'i':
		memcpy(&box, &temp, sizeof(box));
		memcpy(&robot, &temp, sizeof(box));
		memcpy(&robot_arm, &temp, sizeof(box));
		memcpy(&robot_leg, &temp, sizeof(box));
		memcpy(&block, &temp, sizeof(box));
		memcpy(&camera, &temp, sizeof(camera));
		memcpy(&h_f, &temp_f, sizeof(h_f));
		baseRadian = orignalRadian;
		baseSpeed = orignalSpeed;
		Setting();
		break;
	case 'j':
		robot.is_jump = true;
		break;
	case 'w':
		h_f.robot_front_move = true;
		h_f.robot_back_move = false;
		h_f.robot_right_move = false;
		h_f.robot_left_move = false;
		robot.y_rotate = 180.f;
		break;
	case 'a':
		h_f.robot_front_move = false;
		h_f.robot_back_move = false;
		h_f.robot_right_move = false;
		h_f.robot_left_move = true;
		robot.y_rotate = 270.f;
		break;
	case 's':
		h_f.robot_front_move = false;
		h_f.robot_back_move = true;
		h_f.robot_right_move = false;
		h_f.robot_left_move = false;
		robot.y_rotate = 0.f;
		break;
	case 'd':
		h_f.robot_front_move = false;
		h_f.robot_back_move = false;
		h_f.robot_right_move = true;
		h_f.robot_left_move = false;
		robot.y_rotate = 90.f;
		break;
	case 'z':
		h_f.z_is_trans = !h_f.z_is_trans;
		break;
	case 'Z':
		h_f.z_is_reverse_trans = !h_f.z_is_reverse_trans;
		break;
	case 'x':
		h_f.x_is_trans = !h_f.x_is_trans;
		break;
	case 'X':
		h_f.x_is_reverse_trans = !h_f.x_is_reverse_trans;
		break;
	case 'c':
		h_f.robot_see != h_f.robot_see;
		break;
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}



GLvoid Timer(int value)
{

		if (h_f.door_open && box.x_trans < 1.0)
			box.x_trans += 0.01f;
		if (h_f.door_open && box.x_trans1 > -1.0)
			box.x_trans1 -= 0.01f;

		if (h_f.z_is_trans)
			camera.z_trans += 0.01;
		if (h_f.z_is_reverse_trans)
			camera.z_trans -= 0.01;

		if (h_f.x_is_trans)
			camera.x_trans += 0.01;
		if (h_f.x_is_reverse_trans)
			camera.x_trans -= 0.01;

		if (h_f.y_is_rotate)
			camera.y_rotate_aoc += 0.1;
		if (h_f.y_is_reverse_rotate)
			camera.y_rotate_aoc -= 0.1;
		
		if (box.y_trans_aoc < 2)box.y_trans_aoc += 0.01f;

		

		if (h_f.robot_front_move) {
			if (robot.z_trans_aoc <= -0.85)
				robot.z_trans_aoc = -0.85f;
			if (!DoCollision(0, 0, -baseSpeed))robot.z_trans_aoc -= baseSpeed;
		}
		if (h_f.robot_back_move) {
			if (robot.z_trans_aoc >= 0.85)
				robot.z_trans_aoc = 0.85f;
			if (!DoCollision(0, 0, baseSpeed))robot.z_trans_aoc += baseSpeed;
		}
		if (h_f.robot_right_move) {
			if (robot.x_trans_aoc >= 0.85)
				robot.x_trans_aoc = 0.85f;
			if (!DoCollision(baseSpeed, 0, 0))robot.x_trans_aoc += baseSpeed;
		}
		if (h_f.robot_left_move) {
			if (robot.x_trans_aoc <= -0.85)
				robot.x_trans_aoc = -0.85f;
			if (!DoCollision(-baseSpeed, 0, 0))robot.x_trans_aoc -= baseSpeed;
		}
		if (h_f.robot_back_move || h_f.robot_front_move || h_f.robot_left_move || h_f.robot_right_move)
			h_f.is_robot_move = true;
		else h_f.is_robot_move = false;
		if (h_f.is_robot_move) {
			if (robot_leg.x_rotate_aoc > baseRadian && robot_leg.rotate_up)
				robot_leg.rotate_up = !robot_leg.rotate_up;
			if (robot_leg.x_rotate_aoc < -baseRadian && !robot_leg.rotate_up)
				robot_leg.rotate_up = !robot_leg.rotate_up;

			if (robot_leg.rotate_up)
				robot_leg.x_rotate_aoc += 2;
			else robot_leg.x_rotate_aoc -= 2;
		}
		else {
			if (robot_leg.x_rotate_aoc > 0)
				robot_leg.x_rotate_aoc -= 2;
			if (robot_leg.x_rotate_aoc < 0)
				robot_leg.x_rotate_aoc += 2;
		}
		if (robot.is_jump) {
			if (robot.y_trans_aoc > 0.5f && robot.y_trans_up)
				robot.y_trans_up = !robot.y_trans_up;
			if (robot.y_trans_up)
				robot.y_trans_aoc += 0.02f;
			else {
				if (!DoCollision(0, -0.02f, 0))
					robot.y_trans_aoc -= 0.02f;
			}

			if (robot.y_trans_aoc <= 0 || DoCollision(0, -0.02f, 0)) {
				robot.is_jump = false;
				robot.y_trans_up = true;
			}
		}


	if (!DoCollision(0, -0.02, 0) && robot.y_trans_aoc > 0 && !robot.is_jump)
		robot.y_trans_aoc -= 0.02f;

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}

GLvoid Motion(int x, int y)
{
	glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y)
{
	glutPostRedisplay();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("C.G_NO.22");
	Setting();

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cerr << "NOT INIT" << endl;
	}
	else
		cout << "INIT"<<endl;
		cout << "o :문열기" << endl;
		cout << "w,a,s,d : 로봇 조종" << endl;
		cout << "+/- : 걷는 속도 빨라짐/느려짐" << endl;
		cout << "j : 점프" << endl;
		cout << "i : 리셋" << endl;
		cout << "z/Z : 앞뒤로 이동" << endl;
		cout << "x/X : 좌우로 이동" << endl;
		cout << "y/Y : 카메라가 y축으로 공전" << endl;
		cout << "c : 1인칭 시점" << endl;

	InitShader();
	InitBuffer();
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(KeyBoard);
	glutKeyboardUpFunc(KeyBoardUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutReshapeFunc(Reshape);
	glutTimerFunc(10, Timer, 1);
	glutMainLoop();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLvoid Setting()
{
	uniform_int_distribution<int> dis{ -80, 80 };
	block.x_trans = dis(gen) / 100.;
	block.z_trans = dis(gen) / 100.;
	block2.x_trans = dis(gen) / 100.;
	block2.z_trans = dis(gen) / 100.;
	block3.x_trans = dis(gen) / 100.;
	block3.z_trans = dis(gen) / 100.;
}

GLint Collision(float first_x1, float first_x2, float last_x1, float last_x2)
{
	if (first_x1 <= last_x1 && last_x1 <= first_x2)
		return 1;
	if (first_x1 <= last_x2 && last_x2 <= first_x2)
		return 1;
	if (last_x1 <= first_x1 && first_x1 <= last_x2)
		return 1;
	if (last_x1 <= first_x2 && first_x2 <= last_x2)
		return 1;
	return 0;
}

bool DoCollision(float x_trans, float y_trans, float z_trans)
{
	if (Collision(robot.x_trans_aoc + x_trans - 0.04, robot.x_trans_aoc + x_trans + 0.04, block.x_trans - 0.2, block.x_trans + 0.2) +
		Collision(robot.z_trans_aoc + z_trans - 0.04, robot.z_trans_aoc + z_trans + 0.04, block.z_trans - 0.2, block.z_trans + 0.2) +
		Collision(robot.y_trans_aoc + y_trans, robot.y_trans_aoc + y_trans + 0.3, block.y_trans, block.y_trans + 0.14) == 3)
		return true;
	else if (Collision(robot.x_trans_aoc + x_trans - 0.04, robot.x_trans_aoc + x_trans + 0.04, block2.x_trans - 0.2, block2.x_trans + 0.2) +
		Collision(robot.z_trans_aoc + z_trans - 0.04, robot.z_trans_aoc + z_trans + 0.04, block2.z_trans - 0.2, block2.z_trans + 0.2) +
		Collision(robot.y_trans_aoc + y_trans, robot.y_trans_aoc + y_trans + 0.3, block2.y_trans, block2.y_trans + 0.14) == 3)
		return true;
	else if (Collision(robot.x_trans_aoc + x_trans - 0.04, robot.x_trans_aoc + x_trans + 0.04, block3.x_trans - 0.2, block3.x_trans + 0.2) +
		Collision(robot.z_trans_aoc + z_trans - 0.04, robot.z_trans_aoc + z_trans + 0.04, block3.z_trans - 0.2, block3.z_trans + 0.2) +
		Collision(robot.y_trans_aoc + y_trans, robot.y_trans_aoc + y_trans + 0.3, block3.y_trans, block3.y_trans + 0.14) == 3)
		return true;
	else if (Collision(robot.x_trans_aoc + x_trans - 0.04, robot.x_rotate_aoc + x_trans + 0.04, + 0.85f, -0.85f) +
		Collision(robot.z_trans_aoc + z_trans - 0.04, robot.z_trans_aoc + z_trans + 0.04, 0.85f, -0.85f) +
		Collision(robot.y_trans_aoc + y_trans, robot.y_trans_aoc + y_trans + 0.3, 0.f, 0.f) == 2)
		robot.y_rotate = robot.y_rotate + 180;
	else return false;
}

GLvoid view(int value) {
	if (h_f.robot_see) {
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.3f);
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

		view = glm::rotate(view, glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f));

		view = glm::rotate(view, glm::radians(camera.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));

		view = glm::translate(view, glm::vec3(camera.x_trans, camera.y_trans, camera.z_trans));


		unsigned int viewLocation = glGetUniformLocation(s_program, "view");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
		unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
}
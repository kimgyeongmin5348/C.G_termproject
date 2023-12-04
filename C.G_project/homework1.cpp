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

#define width 600
#define height 600
#define h_vertex 0.2f

using namespace std;

random_device rd;
mt19937 gen(rd());

float hexa_tool[] =
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


GLvoid drawScene();
GLvoid Reshape(int, int);
GLvoid KeyBoard(unsigned char, int, int);
GLvoid Setting();
GLvoid make_maze();
GLvoid input_size();
GLvoid hexa_speed(bool);

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
	float x_trans{}, y_trans{}, z_trans{};
	float x_trans_aoc{}, y_trans_aoc{}, z_trans_aoc{};
	float x_scale{}, y_scale{}, z_scale{};
	float y_max_scale{}, y_min_scale{}, y_scale_aoc{};
	float lights_r{}, lights_g, lights_b;

	bool y_scale_up{ true };
}O;

typedef struct function {

	bool y_is_rotate{ false };
	bool y_is_reverse_rotate{ false };
	bool start_timer{ true };
	int x_max{}, z_max{};
	bool hexa_timer{ false };

}F;

O temp, temp_hexa[25][25], camera, hexa[25][25], change;
O l;
F h_f, temp_f;
int count{};
bool change_type{ false };
bool pers{ true };
bool light{ true };


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
		cout << "good" << endl;
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(s_program);
}
GLvoid InitBuffer()
{
	glGenVertexArrays(3, VAO);

	glBindVertexArray(VAO[0]);
	glGenBuffers(2, &VBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hexa_tool), hexa_tool, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);



}
void drawScene()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //깊이 체크 (컬링)
	glUseProgram(s_program);

	for (int i = 0; i < 2; ++i) {

		if (i == 0) {
			glViewport(0, 0, 600, 500);			
			glm::vec3 cameraPos = glm::vec3(0.0f, 1.3f, 1.0f);			//위치
			glm::vec3 cameraDirection = glm::vec3(0.0f, 1.0f, 0.0f);	//바라보는 방향
			glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.f);			//카메라 상향
			glm::mat4 view = glm::mat4(1.0f);
			view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
			view = glm::rotate(view, glm::radians(camera.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
			view = glm::translate(view, glm::vec3(-h_f.x_max / 2 * 0.4f, -h_f.z_max / 2 * 0.2f, -h_f.z_max * 0.7f));
			unsigned int viewLocation = glGetUniformLocation(s_program, "view");
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
			
			glm::mat4 projection = glm::mat4(1.0f);
			projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
			projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
			unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
				
		}
		else {
			glViewport(390, 390, 200, 200);
			glm::vec3 cameraPos = glm::vec3(0.0f, 7.0f, 0.0f);			//위치
			glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);	//바라보는 방향
			glm::vec3 cameraUp = glm::vec3(1.0f, 0.0f, 0.f);			//카메라 상향
			glm::mat4 view = glm::mat4(1.0f);
			view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
			view = glm::rotate(view, glm::radians(camera.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
			unsigned int viewLocation = glGetUniformLocation(s_program, "view");
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
			
			glm::mat4 projection = glm::mat4(1.0f);
			projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
			projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
			unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
			

		}

		if (!light) {
			glUseProgram(s_program);
			unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
			glUniform3f(lightPosLocation, 7.0f, 3.0f, 5.0f);
			unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
			glUniform3f(lightColorLocation, 0.0f, 0.0f, 0.0f);
			unsigned int objColorLocation = glGetUniformLocation(s_program, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
			glUniform3f(objColorLocation, 0.7f, 0.7f, 0.4f);
		}
		else {
			glUseProgram(s_program);
			unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
			glUniform3f(lightPosLocation, 7.0f, 3.0f, 5.0f);
			unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
			glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
			unsigned int objColorLocation = glGetUniformLocation(s_program, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
			glUniform3f(objColorLocation, 0.7f, 0.7f, 0.4f);
			//unsigned int viewPosLocation = glGetUniformLocation(s_program, "viewPos");
			//glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);
		}


		//Hexa
		glm::mat4 H_Matrix = glm::mat4(1.0f);
		for (int i = 0; i < h_f.x_max; ++i) {
			for (int j = 0; j < h_f.z_max; ++j) {
				// 윗면
				H_Matrix = glm::mat4(1.0f);
				H_Matrix = glm::translate(H_Matrix, glm::vec3(hexa[i][j].x_trans, 0.0f, hexa[i][j].z_trans));
				H_Matrix = glm::scale(H_Matrix, glm::vec3(1.0f, hexa[i][j].y_scale, 1.0f));
				H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, 0.2f, 0.f));
				unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
				glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
				qobj = gluNewQuadric();
				gluQuadricDrawStyle(qobj, obj_type);
				int objColorLocation = glGetUniformLocation(s_program, "objectColor");
				unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
				glUniform1f(isCheck, false);
				glUniform4f(objColorLocation, 0.f, 0.5f, 0.5f, 1.0);
				glBindVertexArray(VAO[0]);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				H_Matrix = glm::mat4(1.0f);
				H_Matrix = glm::translate(H_Matrix, glm::vec3(hexa[i][j].x_trans, 0.0f, hexa[i][j].z_trans));
				H_Matrix = glm::scale(H_Matrix, glm::vec3(1.0f, hexa[i][j].y_scale, 1.0f));
				H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, 0.2f, 0.f));
				StransformLocation = glGetUniformLocation(s_program, "transform");
				glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
				qobj = gluNewQuadric();
				gluQuadricDrawStyle(qobj, obj_type);
				objColorLocation = glGetUniformLocation(s_program, "objectColor");
				isCheck = glGetUniformLocation(s_program, "isCheck");
				glUniform1f(isCheck, false);
				glUniform4f(objColorLocation, 0.7f, 0.7f, 0.4f, 1.0);
				glBindVertexArray(VAO[0]);
				glDrawArrays(GL_TRIANGLES, 30, 6);

				H_Matrix = glm::mat4(1.0f);
				H_Matrix = glm::translate(H_Matrix, glm::vec3(hexa[i][j].x_trans, 0.0f, hexa[i][j].z_trans));
				H_Matrix = glm::scale(H_Matrix, glm::vec3(1.0f, hexa[i][j].y_scale, 1.0f));
				H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, 0.2f, 0.f));
				StransformLocation = glGetUniformLocation(s_program, "transform");
				glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
				qobj = gluNewQuadric();
				gluQuadricDrawStyle(qobj, obj_type);
				objColorLocation = glGetUniformLocation(s_program, "objectColor");
				isCheck = glGetUniformLocation(s_program, "isCheck");
				glUniform1f(isCheck, false);
				glUniform4f(objColorLocation, 0.7f, 0.7f, 0.4f, 1.0);
				glBindVertexArray(VAO[0]);
				glDrawArrays(GL_TRIANGLES, 6, 24);
			}
		}
	}

	glutSwapBuffers();
}
void Reshape(int w, int h) {

	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);

}

void change_the_light() {
	uniform_real_distribution<float> color{ 0,1 };
	change.lights_r = color(gen);
	change.lights_g = color(gen);
	change.lights_b = color(gen);
}

GLvoid KeyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'y':
		h_f.y_is_rotate = !h_f.y_is_rotate;
		cout << "'y' - y축 기준 음회전" << endl;
		break;
	case 'Y':
		h_f.y_is_reverse_rotate = !h_f.y_is_reverse_rotate;
		cout << "'Y' - y축 기준 양회전" << endl;
		break;
	case 'q':
		cout << "'q' - 종료" << endl;
		exit(0);
		break;
	case 't':
		light = !light;
		cout << "'t' - 불을 키고 끈다" << endl;
		break;
	case 'c':
		change_the_light();
		cout << " 'c' - 조명색 변경" << endl;
		break;
	case '+':
		hexa_speed(true);
		cout << "'+' - 육면체 속도 증가" << endl;
		break;
	case '-':
		hexa_speed(false);
		cout << "'-' - 육면체 속도 감소" << endl;
		break;
	case '1':
		h_f.hexa_timer = !h_f.hexa_timer;
		cout << "'1' - 에니매이션 1" << endl;
		break;
	case 'r':
		memcpy(&hexa, &temp_hexa, sizeof(hexa));
		memcpy(&camera, &temp, sizeof(camera));
		memcpy(&h_f, &temp_f, sizeof(h_f));
		input_size();
		Setting();
		cout << "'r' - 모든 값 초기화" << endl;
		break;
	}
	glutPostRedisplay();
}


GLvoid Timer(int value)
{
	if (h_f.start_timer) {
		//cam
		if (h_f.y_is_rotate)
			camera.y_rotate_aoc += 0.1;
		if (h_f.y_is_reverse_rotate)
			camera.y_rotate_aoc -= 0.1;

		//hexa
		if (h_f.hexa_timer) {
			for (int i = 0; i < h_f.x_max; ++i) {
				for (int j = 0; j < h_f.z_max; ++j) {
					if (hexa[i][j].y_scale_up) {
						if (hexa[i][j].y_scale + hexa[i][j].y_scale_aoc <= hexa[i][j].y_max_scale) {
							hexa[i][j].y_scale += hexa[i][j].y_scale_aoc;
						}
						else {
							hexa[i][j].y_scale_up = !hexa[i][j].y_scale_up;
						}
					}
					else {
						if (hexa[i][j].y_scale - hexa[i][j].y_scale_aoc >= hexa[i][j].y_min_scale) {
							hexa[i][j].y_scale -= hexa[i][j].y_scale_aoc;
						}
						else {
							hexa[i][j].y_scale_up = !hexa[i][j].y_scale_up;
						}
					}

				}
			}
		}

		glutPostRedisplay();
		glutTimerFunc(10, Timer, 1);
	}
}


void main(int argc, char** argv) {
	input_size();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("Amazing_Movement");
	Setting();

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cerr << "NOT INIT" << endl;
	}
	else
		cout << "INIT" << endl;
	cout << "=============== MENU ===============" << endl;
	cout << "1,2,3 : 애니메이션1,2,3" << endl;
	cout << "t/T : 조명을 키고 끈다" << endl;
	cout << "c : 조명의 색을 바꾼다" << endl;
	cout << "+ / - : 육면체의 속도 조절" << endl;
	cout << "r : 모든값 초기화" << endl;
	cout << "q : 프로그램 종료" << endl;
	cout << "====================================" << endl;

	InitShader();
	InitBuffer();
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(KeyBoard);
	glutReshapeFunc(Reshape);
	change_the_light();
	glutTimerFunc(10, Timer, 1);
	glutMainLoop();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLvoid Setting()
{
	uniform_int_distribution<> dis{ 10,50 };
	uniform_int_distribution<> dis1{ 0,1 };
	for (int i = 0; i < h_f.x_max; ++i) {
		for (int j = 0; j < h_f.z_max; ++j) {
			hexa[i][j].x_trans = i * 0.4f;
			hexa[i][j].z_trans = j * 0.4f;
			hexa[i][j].y_scale = dis(gen) / 20.;
			hexa[i][j].y_scale_aoc = (dis(gen)) / 5000. * 2;
			hexa[i][j].y_max_scale = (dis(gen) + 40) / 10.;
			hexa[i][j].y_min_scale = (dis(gen) - 10) / 10.;
			hexa[i][j].y_scale_up = dis1(gen);
		}
	}
	change.lights_r = 1.0f;
	change.lights_g = 1.0f;
	change.lights_b = 1.0f;



}

GLvoid input_size()
{
	cout << "가로 세로 : ";
	cin >> h_f.x_max >> h_f.z_max;
}


GLvoid hexa_speed(bool is_up)
{
	for (int i = 0; i < h_f.x_max; ++i) {
		for (int j = 0; j < h_f.z_max; ++j) {
			if (is_up)hexa[i][j].y_scale_aoc += 0.01;
			else hexa[i][j].y_scale_aoc -= 0.01;
		}
	}
}
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

#define width 800
#define height 600
#define h_vertex 0.5f

using namespace std;

GLvoid drawScene();
GLvoid Reshape(int, int);
GLvoid KeyBoard(unsigned char, int, int);
GLvoid Setting();

GLUquadricObj* qobj;
GLuint s_program;
GLuint VAO[3], VBO[6];
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;
GLenum hexa_mode{ GL_FILL };
GLint g_window_w, g_window_h;

typedef struct object {
	float x_rotate{}, y_rotate{}, z_rotate{};
	float x_rotA{};
	float x_trans{}, y_trans{}, z_trans{};
	float x_scale{}, y_scale{}, z_scale{};
}O;

typedef struct function {
	bool cull{ false };
	bool y_rotate{ false };
	bool top_spin{ false };
	bool front_open{ false };
	bool side_open{ false };
	bool open{ false };
	bool point{ false };
	bool qside_open{ false };
}F;

O h[6], q[6];
O l;
F h_f;
int count{};
bool change_type{ false };
bool pers{ false };

float line[]
{
	-2,0,0,
	2,0,0,
	0,-2,0,
	0,2,0,
	0,0,-2,
	0,0,2
};

float line_color[]
{
	0,0,1,
	0,0,1,
	0,1,0,
	0,1,0,
	1,0,0,
	1,0,0
};

float hexa[] =
{
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

float hexa_color[] =
{     //육면체
	//윗면
	1,0,0,
	0,1,0,
	1,0,1,
	1,0,0,
	0,1,0,
	0,0,1,

	//정면
	1,0,0,
	0,1,0,
	0,1,1,
	1,1,0,
	1,1,0,
	0,1,1,

	//아랫면
	1,0,1,
	0,1,1,
	0,1,1,
	0,0,1,
	0,1,1,
	1,0,1,

	//뒷면
	1,1,0,
	0,1,0,
	0,1,1,
	0,1,1,
	0,1,0,
	1,0,0,

	//좌측면
	1,0,0,
	0,1,0,
	0,0,1,
	1,0,0,
	0,1,0,
	0,0,1,

	//우측면
	0,0,1,
	0,1,0,
	1,0,0,
	0,0,1,
	0,1,0,
	1,0,0,
};

float quad[] =
{   //사각뿔
   -0.5f, -0.5f, -0.5f,
   0.0f, 0.5f, 0.0f,
   0.5f, -0.5f, -0.5f,

   0.5f, -0.5f, -0.5f,
   0.0f, 0.5f, 0.0f,
   0.5f, -0.5f, 0.5f,

   0.5f, -0.5f, 0.5f,
   0.0f, 0.5f, 0.0f,
   -0.5f, -0.5f, 0.5f,

   -0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, 0.5f,
   0.0f, 0.5f, 0.0f,

   //아래
  -0.5f, -0.5f, 0.5f,
  -0.5f, -0.5f, -0.5f,
  0.5f, -0.5f, -0.5f,

  -0.5f, -0.5f, 0.5f,
  0.5f, -0.5f, -0.5f,
  0.5f, -0.5f, 0.5f,
};

float quad_color[] =
{   //사각뿔
  1,0,0,
  0,1,0,
  0,0,1,

  1,0,0,
  0,1,0,
  0,0,1,

  1,0,0,
  0,1,0,
  0,0,1,

  0,0,1,
  0,1,0,
  1,0,0,

  1,0,0,
  0,1,0,
  0,1,1,

  0,1,0,
  0,0,1,
  1,0,0
};

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

	// 선
	glBindVertexArray(VAO[0]);
	glGenBuffers(2, &VBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	// 선 색
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]); // 색상
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_color), line_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// 정육면체
	glBindVertexArray(VAO[1]);
	glGenBuffers(2, &VBO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hexa), hexa, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	// 정육면체 색
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]); // 색상
	glBufferData(GL_ARRAY_BUFFER, sizeof(hexa_color), hexa_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// 정사면체
	glBindVertexArray(VAO[2]);
	glGenBuffers(2, &VBO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	// 정사면체 색
	glBindBuffer(GL_ARRAY_BUFFER, VBO[5]); // 색상
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_color), quad_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

}

void drawScene()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //깊이 체크 (컬링)
	glUseProgram(s_program);

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.3f);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
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
		projection = glm::translate(projection, glm::vec3(0.0f, 0.0f, -5.0f));
		unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}

	glm::mat4 L_Matrix = glm::mat4(1.0f);
	L_Matrix = glm::translate(L_Matrix, glm::vec3(0.f, 0.f, 0.0f));
	L_Matrix = glm::rotate(L_Matrix, glm::radians(l.x_rotate), glm::vec3(1.0f, 0.0f, 0.0f));
	L_Matrix = glm::rotate(L_Matrix, glm::radians(l.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int HtransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(HtransformLocation, 1, GL_FALSE, glm::value_ptr(L_Matrix));
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_LINES, 0, 6);

	for (int i = 0; i < 6; ++i) {
		if (!change_type) {
			glm::mat4 H_Matrix = glm::mat4(1.0f);

			H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, 0.f, 0.f));
			H_Matrix = glm::rotate(H_Matrix, glm::radians(h[i].x_rotate), glm::vec3(1.0f, 0.0f, 0.0f));
			H_Matrix = glm::rotate(H_Matrix, glm::radians(h[i].y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));

			// 양 옆면 위로 이동
			if (i == 4 || i == 5)H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, h[i].y_trans, 0.f));

			// 윗면 돌리기
			if (i == 0) {
				H_Matrix = glm::translate(H_Matrix, glm::vec3(0.0f, h_vertex, 0.0f));
				H_Matrix = glm::rotate(H_Matrix, glm::radians(h[i].x_rotA), glm::vec3(1.0f, 0.0f, 0.0f));
				H_Matrix = glm::translate(H_Matrix, glm::vec3(0.0f, -h_vertex, 0.0f));
			}
			// 앞면 열기
			if (i == 1) {
				H_Matrix = glm::translate(H_Matrix, glm::vec3(0.0f, -h_vertex, h_vertex));
				H_Matrix = glm::rotate(H_Matrix, glm::radians(h[i].x_rotA), glm::vec3(1.0f, 0.0f, 0.0f));
				H_Matrix = glm::translate(H_Matrix, glm::vec3(0.0f, h_vertex, -h_vertex));
			}
			// 뒷면 열기
			if (i == 3) {
				H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, h_vertex, 0.f));
				H_Matrix = glm::scale(H_Matrix, glm::vec3(h[i].x_scale, h[i].y_scale, h[i].z_scale));
				H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, -h_vertex, 0.f));
			}

			glUniformMatrix4fv(HtransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
			glBindVertexArray(VAO[1]);
			glDrawArrays(GL_TRIANGLES, i * 6, 6);
		}
		else {
			glm::mat4 Q_Matrix = glm::mat4(1.0f);

			Q_Matrix = glm::translate(Q_Matrix, glm::vec3(0.0f, 0.0f, 0.0f));
			Q_Matrix = glm::rotate(Q_Matrix, glm::radians(h[i].x_rotate), glm::vec3(1.0f, 0.0f, 0.0f));
			Q_Matrix = glm::rotate(Q_Matrix, glm::radians(h[i].y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));
			switch (i) {
			case 0:
				Q_Matrix = glm::translate(Q_Matrix, glm::vec3(0.0f, -0.5f, -0.5f));
				Q_Matrix = glm::rotate(Q_Matrix, glm::radians(q[i].x_rotA), glm::vec3(1.0f, 0.0f, 0.0f));
				Q_Matrix = glm::translate(Q_Matrix, glm::vec3(0.0f, 0.5f, 0.5f));

				break;

			case 1:
				Q_Matrix = glm::translate(Q_Matrix, glm::vec3(0.5f, -0.5f, 0.0f));
				Q_Matrix = glm::rotate(Q_Matrix, glm::radians(q[i].x_rotA), glm::vec3(0.0f, 0.0f, 1.0f));
				Q_Matrix = glm::translate(Q_Matrix, glm::vec3(-0.5f, 0.5f, 0.0f));
				break;

			case 2:
				Q_Matrix = glm::translate(Q_Matrix, glm::vec3(0.0f, -0.5f, 0.5f));
				Q_Matrix = glm::rotate(Q_Matrix, glm::radians(q[i].x_rotA), glm::vec3(1.0f, 0.0f, 0.0f));
				Q_Matrix = glm::translate(Q_Matrix, glm::vec3(0.0f, 0.5f, -0.5f));
				break;

			case 3:
				Q_Matrix = glm::translate(Q_Matrix, glm::vec3(-0.5f, -0.5f, 0.0f));
				Q_Matrix = glm::rotate(Q_Matrix, glm::radians(q[i].x_rotA), glm::vec3(0.0f, 0.0f, 1.0f));
				Q_Matrix = glm::translate(Q_Matrix, glm::vec3(0.5f, 0.5f, 0.0f));

				break;


			}

			glUniformMatrix4fv(HtransformLocation, 1, GL_FALSE, glm::value_ptr(Q_Matrix));
			glBindVertexArray(VAO[2]);
			glDrawArrays(GL_TRIANGLES, i * 3, 3);
		}
	}

	glutSwapBuffers();
}

void Reshape(int w, int h) {

	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);

}

GLvoid KeyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'y':
		h_f.y_rotate = !h_f.y_rotate;
		break;
	case 'h':
		h_f.cull = !h_f.cull;
		if (h_f.cull)glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		break;
	case 't':
		h_f.top_spin = !h_f.top_spin;
		break;
	case 'f':
		h_f.front_open = !h_f.front_open;
		break;
	case 's':
		h_f.side_open = !h_f.side_open;
		break;
	case'b':
		h_f.point = !h_f.point;
		break;
	case 'c':
		change_type = !change_type;
		break;
	case'r':
		h_f.qside_open = !h_f.qside_open; // 이건 도저히 못하겠음...
		break;
	case 'o':
		h_f.open = !h_f.open;
		break;
	case 'p':
		pers = !pers;
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value)
{

	if (h_f.y_rotate)
		for (O& i : h)
			i.y_rotate += 0.2f;

	if (h_f.top_spin)
		h[0].x_rotA += 0.5f;

	if (h_f.front_open && h[1].x_rotA < 90.)
		h[1].x_rotA += 0.2f;
	else if (!h_f.front_open && h[1].x_rotA > 0.)
		h[1].x_rotA -= 0.2f;

	if (h_f.side_open && h[4].y_trans < 1.0f) {
		h[4].y_trans += 0.004f;
		h[5].y_trans += 0.004f;
	}
	else if (!h_f.side_open && h[4].y_trans > 0.) {
		h[4].y_trans -= 0.004f;
		h[5].y_trans -= 0.004f;
	}
	if (h_f.point) {
		for (int i = 0; i < 6; ++i) {
			if (h[i].x_scale > 0) {
				h[i].x_scale -= 0.01f;
				h[i].y_scale -= 0.01f;
				h[i].z_scale -= 0.01f;
			}
		}
	}
	else {
		for (int i = 0; i < 6; ++i) {
			if (h[i].x_scale < 1) {
				h[i].x_scale += 0.01f;
				h[i].y_scale += 0.01f;
				h[i].z_scale += 0.01f;
			}
		}
	}
	if (h_f.open) {
		for (int i = 0; i < 4; ++i) {
			switch (i) {
			case 1:
				if (q[i].x_rotA > -232.2)q[i].x_rotA -= 0.5f;
				break;
			case 2:
				if (q[i].x_rotA < 233.2)q[i].x_rotA += 0.5f;
				break;
			case 3:
				if (q[i].x_rotA < 233.2)q[i].x_rotA += 0.5f;
				break;
			case 0:
				if (q[i].x_rotA > -233.2)q[i].x_rotA -= 0.5f;
				break;
			}
		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			switch (i) {
			case 1:
				if (q[i].x_rotA < 0)q[i].x_rotA += 0.5f;
				break;
			case 2:
				if (q[i].x_rotA > 0)q[i].x_rotA -= 0.5f;
				break;
			case 3:
				if (q[i].x_rotA > 0)q[i].x_rotA -= 0.5f;
				break;
			case 0:
				if (q[i].x_rotA < 0)q[i].x_rotA += 0.5f;
				break;
			}
		}
	}
	if (h_f.qside_open) {

	}


	glutPostRedisplay();
	glutTimerFunc(5, Timer, 1);
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
	glutInitWindowSize(800, 600);
	glutCreateWindow("C.G_NO18");
	Setting();

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cerr << "NOT INIT" << endl;
	}
	else
		cout << "INIT<<endl"<<endl;
		cout << "y y축으로 회전" << endl;
		cout << "t 윗면 회전" << endl;
		cout << "h 뒷면 보이게" << endl;
		cout << "s 양 옆면 올리기" << endl;
		cout << "f 앞면 열기" << endl;
		cout << "b 뒷면 열기" << endl;
		cout << "c 도형 변경" << endl;
		cout << "o 사각뿔 모든옆면 열기" << endl;

	InitShader();
	InitBuffer();
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(KeyBoard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutReshapeFunc(Reshape);
	glutTimerFunc(5, Timer, 1);
	glutMainLoop();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLvoid Setting()
{
	for (int i = 0; i < 6; ++i) {
		h[i].x_rotate = 30.f;
		h[i].y_rotate = -30.f;
	}
	l.x_rotate = h[0].x_rotate;
	l.y_rotate = h[0].y_rotate;
}

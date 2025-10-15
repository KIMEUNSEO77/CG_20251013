#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <freeglut_ext.h> 
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLint width = 800, height = 800;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint axisVAO = 0, axisVBO = 0;   // ���
GLuint cubeVAO[6] = { 0, }, cubeVBO[6] = { 0 };   // ������ü
GLuint pyramidVAO[6] = { 0, }, pyramidVBO[6] = { 0 };   // �ﰢ��

bool cubeMode = true;         // ������ü ���, fasle�簢��
bool cullMode = false;         // �� ���� ���

float angleX = -30, angleY = 30; // ȸ�� ����
bool rotatingX = false;
bool rotatingY = false;
bool directionX = true; // ȸ�� ���� (true: �ð�, false: �ݽð�)
bool directionY = true;

bool backfaceCulling = true;

// �� ������ (��ġ, ����)
GLfloat axisVertices[] =
{
	// x��
	-0.7f, 0.0f, 0.0f,   1,0,0,
	0.7f, 0.0f, 0.0f,   1,0,0,
	// y��
	0.0f, -0.7f, 0.0f,   0,1,0,
	0.0f, 0.7f, 0.0f,   0,1,0,
	// z��
	0.0f, 0.0f, -0.7f,   0,0,1,
	0.0f, 0.0f, 0.8f,   0,0,1,
};

// ������ü vertex ��ǥ��
float cube[8][3] =
{
	{0.5f, 0, -0.5f}, {0, 0, -0.5f}, {0, 0, 0}, {0.5f, 0, 0},
	{0.5f, 0.5f, -0.5f}, {0, 0.5f, -0.5f}, {0, 0.5f, 0}, {0.5f, 0.5f, 0}
};
int faces[6][4] = {
	{0, 1, 2, 3},
	{4, 7, 6, 5}, 
	{1, 5, 6, 2}, 
	{0, 3, 7, 4}, 
	{0, 4, 5, 1}, 
	{3, 2, 6, 7}  
};
float faceColors[6][3] = {
	{1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {1,0,1}, {0,1,1}   // ����
};

// �ﰢ�� vertex ��ǥ��
float pyramid[5][3] =
{
	{0, 0.5f, 0}, {0.25f, 0, -0.25f}, {-0.25f, 0, -0.25f}, {-0.25f, 0, 0.25f},
	{0.25f, 0, 0.25f}
};
int pyramidFaces[6][3] = {
	{1, 2, 3}, {1, 3, 4}, {0, 3, 2}, {0, 2, 1}, {0, 4, 3}, {0, 1, 4}  // 0, 1�� �� ��
};
float pyramidColors[6][3] = {
	{1,0,0}, {1,0,0}, {0,0,1}, {1,1,0}, {1,0,1}, {0,1,1}   // ����
};

void InitCube()
{
	for (int i = 0; i < 6; i++)   // 6�� ����
	{
		std::vector<GLfloat> vertices;
		int v0 = faces[i][0];
		int v1 = faces[i][1];
		int v2 = faces[i][2];
		int v3 = faces[i][3];
		float* color = faceColors[i];

		// �ﰢ�� 1: v0, v1, v2
		vertices.push_back(cube[v0][0]); vertices.push_back(cube[v0][1]); vertices.push_back(cube[v0][2]);
		vertices.push_back(color[0]); vertices.push_back(color[1]); vertices.push_back(color[2]);
		vertices.push_back(cube[v1][0]); vertices.push_back(cube[v1][1]); vertices.push_back(cube[v1][2]);
		vertices.push_back(color[0]); vertices.push_back(color[1]); vertices.push_back(color[2]);
		vertices.push_back(cube[v2][0]); vertices.push_back(cube[v2][1]); vertices.push_back(cube[v2][2]);
		vertices.push_back(color[0]); vertices.push_back(color[1]); vertices.push_back(color[2]);

		// �ﰢ�� 2: v0, v2, v3
		vertices.push_back(cube[v0][0]); vertices.push_back(cube[v0][1]); vertices.push_back(cube[v0][2]);
		vertices.push_back(color[0]); vertices.push_back(color[1]); vertices.push_back(color[2]);
		vertices.push_back(cube[v2][0]); vertices.push_back(cube[v2][1]); vertices.push_back(cube[v2][2]);
		vertices.push_back(color[0]); vertices.push_back(color[1]); vertices.push_back(color[2]);
		vertices.push_back(cube[v3][0]); vertices.push_back(cube[v3][1]); vertices.push_back(cube[v3][2]);
		vertices.push_back(color[0]); vertices.push_back(color[1]); vertices.push_back(color[2]);

		glGenVertexArrays(1, &cubeVAO[i]);
		glGenBuffers(1, &cubeVBO[i]);
		glBindVertexArray(cubeVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		// ��ġ
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// ����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

	void InitPyramid()
	{
		for (int i = 0; i < 6; i++)   // 6�� ����
		{
			std::vector<GLfloat> vertices;
			int v0 = pyramidFaces[i][0];
			int v1 = pyramidFaces[i][1];
			int v2 = pyramidFaces[i][2];
			float* color = pyramidColors[i];
			// �ﰢ�� 1: v0, v1, v2
			vertices.push_back(pyramid[v0][0]); vertices.push_back(pyramid[v0][1]); vertices.push_back(pyramid[v0][2]);
			vertices.push_back(color[0]); vertices.push_back(color[1]); vertices.push_back(color[2]);
			vertices.push_back(pyramid[v1][0]); vertices.push_back(pyramid[v1][1]); vertices.push_back(pyramid[v1][2]);
			vertices.push_back(color[0]); vertices.push_back(color[1]); vertices.push_back(color[2]);
			vertices.push_back(pyramid[v2][0]); vertices.push_back(pyramid[v2][1]); vertices.push_back(pyramid[v2][2]);
			vertices.push_back(color[0]); vertices.push_back(color[1]); vertices.push_back(color[2]);
			glGenVertexArrays(1, &pyramidVAO[i]);
			glGenBuffers(1, &pyramidVBO[i]);
			glBindVertexArray(pyramidVAO[i]);
			glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
			// ��ġ
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// ����
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
	}

void InitAxis()
{
	glGenVertexArrays(1, &axisVAO);
	glGenBuffers(1, &axisVBO);

	glBindVertexArray(axisVAO);
	glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

	// ��ġ
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
	// Open file for reading 
	// Return NULL on failure 
	// Seek to the end of the file 
	// Find out how many bytes into the file we are 
	// Allocate a buffer for the entire length of the file and a null terminator 
	// Go back to the beginning of the file 
	// Read the contents of the file in to the buffer 
	// Close the file 
	// Null terminator 
	// Return the buffer 
}

void Timer(int value)
{
	if (rotatingX)
	{
		if (directionX) angleX += 1.0f;  // ȸ�� �ӵ�
		else angleX -= 1.0f;

		glutPostRedisplay();
	}
	else if (rotatingY)
	{
		if (directionY) angleY += 1.0f;  // ȸ�� �ӵ�
		else angleY -= 1.0f;
		glutPostRedisplay();
	}
	glutTimerFunc(16, Timer, 0);
}

void Reset()
{
	angleX = -30; angleY = 30;
	rotatingX = false; rotatingY = false;
	directionX = true; directionY = true;
	cubeMode = true;
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'p':
		cubeMode = !cubeMode;
		glutPostRedisplay();
		break;
	case 'h':
		cullMode = !cullMode;
		if (cullMode) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		glutPostRedisplay();
		break;
		// �޸����� ����, ����
		/*
	case 'u':
		backfaceCulling = !backfaceCulling;
		if (backfaceCulling)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else
		{
			glDisable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		glutPostRedisplay();
		break;
		*/
	case 'y':
		rotatingY = !rotatingY;
		directionY = true;
		break;
	case 'c':
		Reset();
		break;
	case 'q':
		exit(0);
		break;
	}
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  // ���� ���� �߰�
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Tesk_17");

	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	InitAxis();   // �� �ʱ�ȭ
	InitCube();   // ������ü �ʱ�ȭ
	InitPyramid();   // �ﰢ�� �ʱ�ȭ

	glEnable(GL_DEPTH_TEST); // ���� �׽�Ʈ Ȱ��ȭ
	//glEnable(GL_CULL_FACE); // �� ����
	//glDepthFunc(GL_ALWAYS);   // �׻� ���

	glutTimerFunc(0, Timer, 0); // Ÿ�̸� �ݹ� ���

	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "Error: vertex shader            \n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader            \n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();

	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);

	glLinkProgram(shaderID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program          \n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}

GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // ���� ���� ���۵� �ʱ�ȭ

	glUseProgram(shaderProgramID);

	// �� ��ȯ ���� (30���� ȸ��)
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(moveX, moveY, 0.0f));  // �̵�
	model = glm::rotate(model, glm::radians(angleX), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(angleY), glm::vec3(0, 1, 0));

	GLuint modelLoc = glGetUniformLocation(shaderProgramID, "uModel");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

	if (cubeMode)
	{
		// �� �麰�� �׸���
		for (int i = 0; i < 6; ++i)
		{
			glBindVertexArray(cubeVAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}

	else if (!cubeMode)
	{
		// �ﰢ�� �� �� �׸���
		for (int i = 0; i < 6; ++i)
		{
			glBindVertexArray(pyramidVAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, 3); // �� ���� �ﰢ�� 1��(3����)
			glBindVertexArray(0);
		}
	}

	// 2. ��ǥ�� �׸� ��: �������(��ȯ ����) ����
	glm::mat4 axisModel = glm::mat4(1.0f);
	axisModel = glm::rotate(axisModel, glm::radians(-30.0f), glm::vec3(1, 0, 0));
	axisModel = glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(0, 1, 0));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &axisModel[0][0]);
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}
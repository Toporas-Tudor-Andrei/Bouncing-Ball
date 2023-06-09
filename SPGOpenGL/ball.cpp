#include "ball.h"
#include "bezier.h"

int ball_wind;

GLuint shader_programme, vao;
glm::mat4 projectionMatrix, viewMatrix, modelMatrix;

glm::vec3 lightPos(0, 1, 5);
glm::vec3 viewPos(2, 3, 6);

SphereMesh sphere(3);
int sphereElementCount = (GLsizei)sphere.triangles.size() * sizeof(glm::ivec3);

std::string textFileRead_ball(char* fn)
{
	std::ifstream ifile(fn);
	std::string filetext;
	while (ifile.good()) {
		std::string line;
		std::getline(ifile, line);
		filetext.append(line + "\n");
	}
	return filetext;
}

void printShaderInfoLog_ball(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog_ball(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

float rotAngle = 0;
float rotAngleInc = PI / 64;
float elevation = 0;
float elevationInc = 0.05f;

void display_ball()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader_programme);

	modelMatrix = glm::mat4();

	glBindVertexArray(vao);

	GLuint lightPosLoc = glGetUniformLocation(shader_programme, "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

	GLuint viewPosLoc = glGetUniformLocation(shader_programme, "viewPos");
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

	modelMatrix *= glm::rotate(rotAngle, glm::vec3(0, 1, 0));
	modelMatrix *= glm::translate(glm::vec3(0, elevation, 0));
	modelMatrix *= glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
	GLuint modelMatrixLoc = glGetUniformLocation(shader_programme, "mvpMatrix");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix));

	// se determina matricea ce realizeaza corectia normalelor. Ea se trimite catre vertex shader la fel cum s-a procedat si cu mvpMatrix 
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	GLuint normalMatrixloc = glGetUniformLocation(shader_programme, "normalMatrix");
	glUniformMatrix4fv(normalMatrixloc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	glDrawElements(GL_TRIANGLES, sphereElementCount, GL_UNSIGNED_INT, NULL);

	glFlush();
}

void init_ball()
{
	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glClearColor(1, 1, 1, 0);
	glEnable(GL_DEPTH_TEST);

	glewInit();

	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sphere.vertices.size() * sizeof(glm::vec3), sphere.vertices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereElementCount, sphere.triangles.data(), GL_STATIC_DRAW);


	std::string vstext = textFileRead_ball("pixel_light.vert");
	std::string fstext = textFileRead_ball("pixel_light.frag");

	const char* vertex_shader = vstext.c_str();
	const char* fragment_shader = fstext.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	printShaderInfoLog_ball(shader_programme);
	printProgramInfoLog_ball(shader_programme);
}

void reshape_ball(int w, int h)
{
	glViewport(0, 0, w, h);
	projectionMatrix = glm::perspective(PI / 6, (float)w / h, 0.1f, 100.0f);
	/*
	viewMatrix este matricea transformarii de observare. Parametrii functiei
	lookAt sunt trei vectori ce reprezinta, in ordine:
	- pozitia observatorului
	- punctul catre care priveste observatorul
	- directia dupa care este orientat observatorul
	*/
	viewMatrix = glm::lookAt(viewPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}
int delay = 7;
int i = 1;
bool stop = true;

void postAllWindowsRedisplay() {
	glutSetWindow(ball_wind);
	glutPostRedisplay();
	glutSetWindow(bezier_wind);
	glutPostRedisplay();
}

void startMovement(int value) {
	stop = false;
}

void move(int value) {
	if (i >= (points.size() - 2))
		i = 1;
	elevation = points[i] / 5;
	updateReference();
	i += 3;
	postAllWindowsRedisplay();
	if (!stop)
	{
		glutTimerFunc(delay, move, 0);
	}
}

void mouse_ball(int button, int state, int x, int y) {
	if (state == GLUT_UP) return;
	switch (button) {
	case GLUT_LEFT_BUTTON:
		stop = true;
		glutTimerFunc(delay, startMovement, 0);
		glutTimerFunc(delay + 5, move, 0);
		break;
	case GLUT_RIGHT_BUTTON:
		i = 1;
		stop = true;
		break;
	case GLUT_MIDDLE_BUTTON:
		stop = true;
		break;
	case 3:
		delay--;
		break;
	case 4:
		delay++;
		break;
	}
	updateReference();
	glutPostRedisplay();
}
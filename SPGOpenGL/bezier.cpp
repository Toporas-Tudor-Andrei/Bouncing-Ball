#include "bezier.h"

GLuint shader_programme_bezier, vao_bezier, ctrlVAO;
GLuint vbo_bezier = 1, ctrlVBO = 2;
glm::mat4 projectionMatrix_bezier, viewMatrix_bezier;
std::vector<float> points;
std::vector<float> ctrlPoints;
glm::vec3 p0, p1, p2, p3;

std::string textFileRead_bezier(char* fn)
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

void display_bezier()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shader_programme_bezier);

	glm::mat4 modelMatrix;

	glBindVertexArray(vao_bezier);
	GLuint matrixID = glGetUniformLocation(shader_programme_bezier, "modelViewProjectionMatrix");

	glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(projectionMatrix_bezier * viewMatrix_bezier * modelMatrix));
	glDrawArrays(GL_LINE_STRIP, 0, points.size() / 3);

	glBindVertexArray(ctrlVAO);
	glDrawArrays(GL_TRIANGLES, 0, points.size() / 3);

	glFlush();
}

float B0(float u) {
	return std::powf(1.0f - u, 3);
}

float B1(float u) {
	return 3 * u * std::powf(1.0f - u, 2);
}

float B2(float u) {
	return 3 * std::powf(u, 2) * (1.0f - u);
}

float B3(float u) {
	return std::powf(u, 3);
}

void createPointsVector()
{
	//p0: 0, -4.8, 0;
	//p1: 1.1, 2.2, 0;
	//p2: 2.3, 6.2, 0;
	//p3: 10, 6.20, 0
	p0 = glm::vec3(0.0f, -4.8f, 0.0f);
	p1 = glm::vec3(1.1f, 2.2f, 0.0f);
	p2 = glm::vec3(2.3f, 6.2f, 0.0f);
	p3 = glm::vec3(10.0f, 6.2f, 0.0f);
	for (float u = 0.0f; u <= 1.0f; u += 0.01f) {
		float x = B0(u) * (-p3.x) + B1(u) * (-p2.x) + B2(u) * (-p1.x) + B3(u) * p0.x;
		float y = B0(u) * p3.y + B1(u) * p2.y + B2(u) * p1.y + B3(u) * p0.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f);
	}
	for (float u = 0.0f; u <= 1.0f; u += 0.01f) {
		float x = B0(u) * p0.x + B1(u) * p1.x + B2(u) * p2.x + B3(u) * p3.x;
		float y = B0(u) * p0.y + B1(u) * p1.y + B2(u) * p2.y + B3(u) * p3.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f);
	}
}

void addControlCircle(float x, float y, float z = 0) {
	float radius = 0.3f;
	for (float angle = 0.0f; angle < PI_BEZIER * 2.0f; angle += PI_BEZIER / 3.0f) {
		ctrlPoints.push_back(x);
		ctrlPoints.push_back(y);
		ctrlPoints.push_back(z);

		ctrlPoints.push_back(radius * std::cosf(angle) + x);
		ctrlPoints.push_back(radius * std::sinf(angle) + y);
		ctrlPoints.push_back(z);

		ctrlPoints.push_back(radius * std::cosf(angle + PI_BEZIER / 3.0f) + x);
		ctrlPoints.push_back(radius * std::sinf(angle + PI_BEZIER / 3.0f) + y);
		ctrlPoints.push_back(z);
	}
}

void createControlPointsVector()
{
	if (!ctrlPoints.empty()) {
		ctrlPoints.clear();
	}
	addControlCircle(-p3.x, p3.y);
	addControlCircle(-p2.x, p2.y);
	addControlCircle(-p1.x, p1.y);
	addControlCircle(p0.x, p0.y);
	addControlCircle(p1.x, p1.y);
	addControlCircle(p2.x, p2.y);
	addControlCircle(p3.x, p3.y);
}

void refresh_bezier() {
	int i = 0;
	for (float u = 0.0f; u <= 1.0f; u += 0.01f) {
		float x = B0(u) * (-p3.x) + B1(u) * (-p2.x) + B2(u) * (-p1.x) + B3(u) * p0.x;
		float y = B0(u) * p3.y + B1(u) * p2.y + B2(u) * p1.y + B3(u) * p0.y;
		points[i] = x;
		i++;
		points[i] = y;
		i += 2;
	}
	for (float u = 0.0f; u <= 1.0f; u += 0.01f) {
		float x = B0(u) * p0.x + B1(u) * p1.x + B2(u) * p2.x + B3(u) * p3.x;
		float y = B0(u) * p0.y + B1(u) * p1.y + B2(u) * p2.y + B3(u) * p3.y;
		points[i] = x;
		i++;
		points[i] = y;
		i += 2;
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bezier);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);
}

void refreshControlCircles() {
	createControlPointsVector();
	glBindBuffer(GL_ARRAY_BUFFER, ctrlVBO);
	glBufferData(GL_ARRAY_BUFFER, ctrlPoints.size() * sizeof(float), &ctrlPoints[0], GL_STATIC_DRAW);
}

void init_bezier()
{
	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glClearColor(1, 1, 1, 0);

	glewInit();

	glGenBuffers(1, &vbo_bezier);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bezier);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ctrlVBO);
	glBindBuffer(GL_ARRAY_BUFFER, ctrlVBO);
	glBufferData(GL_ARRAY_BUFFER, ctrlPoints.size() * sizeof(float), &ctrlPoints[0], GL_STATIC_DRAW);

	ctrlVAO = 1;
	glGenVertexArrays(1, &ctrlVAO);
	glBindVertexArray(ctrlVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, ctrlVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	vao_bezier = 0;
	glGenVertexArrays(1, &vao_bezier);
	glBindVertexArray(vao_bezier);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bezier);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	std::string vstext = textFileRead_bezier("vertex.vert");
	std::string fstext = textFileRead_bezier("fragment.frag");
	const char* vertex_shader = vstext.c_str();
	const char* fragment_shader = fstext.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	shader_programme_bezier = glCreateProgram();
	glAttachShader(shader_programme_bezier, fs);
	glAttachShader(shader_programme_bezier, vs);
	glLinkProgram(shader_programme_bezier);
}

void reshape_bezier(int w, int h)
{
	glViewport(0, 0, w, h);
	projectionMatrix_bezier = glm::perspective(PI_BEZIER / 3, (float)w / h, 0.1f, 100.0f);
	/*
	viewMatrix este matricea transformarii de observare. Parametrii functiei
	lookAt sunt trei vectori ce reprezinta, in ordine:
	- pozitia observatorului
	- punctul catre care priveste observatorul
	- directia dupa care este orientat observatorul
	*/
	viewMatrix_bezier = glm::lookAt(glm::vec3(0, 0, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void keyboard_bezier(unsigned char key, int x, int y)
{
	switch (key)
	{
		//primul punct de control
	case 'a':
		p0.y -= 0.1f;
		break;
	case 'q':
		p0.y += 0.1f;
		break;
		//al doilea punct de control
	case 's':
		p1.x -= 0.1f;
		break;
	case 'f':
		p1.x += 0.1f;
		break;
	case 'e':
		p1.y += 0.1f;
		break;
	case 'd':
		p1.y -= 0.1f;
		break;
		//altreilea punct de control
	case 'g':
		p2.x -= 0.1f;
		break;
	case 'j':
		p2.x += 0.1f;
		break;
	case 'y':
		p2.y += 0.1f;
		break;
	case 'h':
		p2.y -= 0.1f;
		break;
		//al patrulea punct de control
	case 'k':
		p3.y -= 0.1f;
		break;
	case 'i':
		p3.y += 0.1f;
		break;
	case 'o':
		p3.x -= 0.1f;
		break;
	case 'p':
		p3.x += 0.1f;
		break;
	}
	refresh_bezier();
	refreshControlCircles();
	glutPostRedisplay();
}

void mouse_bezier(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		std::cout << " p0.x:" << p0.x << " p0.y:" << p0.y << " p0.z:" << p0.z << std::endl;
		std::cout << " p1.x:" << p1.x << " p1.y:" << p1.y << " p1.z:" << p1.z << std::endl;
		std::cout << " p2.x:" << p2.x << " p2.y:" << p2.y << " p2.z:" << p2.z << std::endl;
		std::cout << " p3.x:" << p3.x << " p3.y:" << p3.y << " p3.z:" << p3.z << std::endl;
		std::cout << "======================================================" << std::endl;
	}
}
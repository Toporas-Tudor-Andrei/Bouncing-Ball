#include "bezier.h"
#include "ball.h"

int bezier_wind;

GLuint shader_programme_bezier, vao_bezier, ctrlVAO, referenceVAO, borderVAO, tangentsVAO;
GLuint vbo_bezier, ctrlVBO, referenceVBO, borderVBO, tangentsVBO;
glm::mat4 projectionMatrix_bezier, viewMatrix_bezier;
std::vector<float> points;
std::vector<float> ctrlPoints;
std::vector<float> referenceP;
std::vector<float> tgLineP;
glm::vec3 p0, p1, p2, p3, pm3, pm2, pm1;
int width, height;
int selectedPoint = -1;
float maxY = 6.5f; // magic numbers for not letting the ball be out of screen
float minY = -8.5f;
float maxX = 15.0f;
float minX = -15.0f;
std::vector<float> bounds = { minX,maxY,0.0f, maxX,maxY,0.0f, maxX,minY,0.0f, minX,minY,0.0f, minX,maxY,0.0f };

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
	glDrawArrays(GL_TRIANGLES, 0, ctrlPoints.size() / 3);

	glBindVertexArray(referenceVAO);
	glDrawArrays(GL_TRIANGLES, 0, referenceP.size() / 3);

	glBindVertexArray(borderVAO);
	glLineWidth(2);
	glDrawArrays(GL_LINE_STRIP, 0, bounds.size() / 3);
	glLineWidth(1);

	glBindVertexArray(tangentsVAO);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1,0x00ff);
	glDrawArrays(GL_LINES, 0, tgLineP.size() / 3);
	glDisable(GL_LINE_STIPPLE);

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
	p0 = glm::vec3(0.0f, -7.45f, 0.0f);
	p1 = glm::vec3(0.38f, 2.3f, 0.0f);
	p2 = glm::vec3(1.3f, 6.15f, 0.0f);
	p3 = glm::vec3(9.38f, 5.92f, 0.0f);
	pm3 = glm::vec3(-9.38f, 5.92f, 0.0f);
	pm2 = glm::vec3(-1.3f, 6.15f, 0.0f);
	pm1 = glm::vec3(-0.38f, 2.3f, 0.0f);
	for (float u = 0.0f; u <= 1.0f; u += 0.01f) {
		float x = B0(u) * (pm3.x) + B1(u) * (pm2.x) + B2(u) * (pm1.x) + B3(u) * p0.x;
		float y = B0(u) * pm3.y + B1(u) * pm2.y + B2(u) * pm1.y + B3(u) * p0.y;
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

void addTangentLinePoints() {
	if (!tgLineP.empty())tgLineP.clear();

	tgLineP.push_back(pm3.x);
	tgLineP.push_back(pm3.y);
	tgLineP.push_back(pm3.z);

	tgLineP.push_back(pm2.x);
	tgLineP.push_back(pm2.y);
	tgLineP.push_back(pm2.z);

	tgLineP.push_back(pm1.x);
	tgLineP.push_back(pm1.y);
	tgLineP.push_back(pm1.z);

	tgLineP.push_back(p0.x);
	tgLineP.push_back(p0.y);
	tgLineP.push_back(p0.z);

	tgLineP.push_back(p0.x);
	tgLineP.push_back(p0.y);
	tgLineP.push_back(p0.z);

	tgLineP.push_back(p1.x);
	tgLineP.push_back(p1.y);
	tgLineP.push_back(p1.z);

	tgLineP.push_back(p2.x);
	tgLineP.push_back(p2.y);
	tgLineP.push_back(p2.z);

	tgLineP.push_back(p3.x);
	tgLineP.push_back(p3.y);
	tgLineP.push_back(p3.z);
}

void refreshTangentLinePoints() {
	addTangentLinePoints();
	glBindBuffer(GL_ARRAY_BUFFER, tangentsVBO);
	glBufferData(GL_ARRAY_BUFFER, tgLineP.size() * sizeof(float), &tgLineP[0], GL_DYNAMIC_DRAW);
}

void addControlCircle(float x, float y, float z = 0.0f) {
	float radius = 0.5f;
	constexpr float offset = PI_BEZIER / 4.0f;
	for (float angle = 0.0f; angle < PI_BEZIER * 2.0f; angle += offset) {
		ctrlPoints.push_back(x);
		ctrlPoints.push_back(y);
		ctrlPoints.push_back(z);

		ctrlPoints.push_back(radius * std::cosf(angle) + x);
		ctrlPoints.push_back(radius * std::sinf(angle) + y);
		ctrlPoints.push_back(z);

		ctrlPoints.push_back(radius * std::cosf(angle + offset) + x);
		ctrlPoints.push_back(radius * std::sinf(angle + offset) + y);
		ctrlPoints.push_back(z);
	}
}

void createControlPointsVector()
{
	if (!ctrlPoints.empty()) {
		ctrlPoints.clear();
	}
	addControlCircle(pm3.x, pm3.y);
	addControlCircle(pm2.x, pm2.y);
	addControlCircle(pm1.x, pm1.y);
	addControlCircle(p0.x, p0.y);
	addControlCircle(p1.x, p1.y);
	addControlCircle(p2.x, p2.y);
	addControlCircle(p3.x, p3.y);
}

void refresh_bezier() {
	int i = 0;
	for (float u = 0.0f; u <= 1.0f; u += 0.01f) {
		float x = B0(u) * (pm3.x) + B1(u) * (pm2.x) + B2(u) * (pm1.x) + B3(u) * p0.x;
		float y = B0(u) * pm3.y + B1(u) * pm2.y + B2(u) * pm1.y + B3(u) * p0.y;
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
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_DYNAMIC_DRAW);
}

void refreshControlCircles() {
	createControlPointsVector();
	glBindBuffer(GL_ARRAY_BUFFER, ctrlVBO);
	glBufferData(GL_ARRAY_BUFFER, ctrlPoints.size() * sizeof(float), &ctrlPoints[0], GL_DYNAMIC_DRAW);
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
	addTangentLinePoints();

	glGenBuffers(1, &vbo_bezier);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bezier);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_DYNAMIC_DRAW);

	glGenBuffers(1, &ctrlVBO);
	glBindBuffer(GL_ARRAY_BUFFER, ctrlVBO);
	glBufferData(GL_ARRAY_BUFFER, ctrlPoints.size() * sizeof(float), &ctrlPoints[0], GL_DYNAMIC_DRAW);

	glGenBuffers(1, &referenceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, referenceVBO);
	glBufferData(GL_ARRAY_BUFFER, referenceP.size() * sizeof(float), &referenceP[0], GL_DYNAMIC_DRAW);

	glGenBuffers(1, &borderVBO);
	glBindBuffer(GL_ARRAY_BUFFER, borderVBO);
	glBufferData(GL_ARRAY_BUFFER, bounds.size() * sizeof(float), &bounds[0], GL_STATIC_DRAW);

	glGenBuffers(1, &tangentsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, tangentsVBO);
	glBufferData(GL_ARRAY_BUFFER, tgLineP.size() * sizeof(float), &tgLineP[0], GL_DYNAMIC_DRAW);

	borderVAO = 4;
	glGenVertexArrays(1, &tangentsVAO);
	glBindVertexArray(tangentsVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, tangentsVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	borderVAO = 3;
	glGenVertexArrays(1, &borderVAO);
	glBindVertexArray(borderVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, borderVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	referenceVAO = 2;
	glGenVertexArrays(1, &referenceVAO);
	glBindVertexArray(referenceVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, referenceVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

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
	width = w;
	height = h;
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

void createReference() {
	static glm::vec3 ref;
	ref.x = points[(size_t)i - 1];
	ref.y = points[i];
	ref.z = points[(size_t)i + 1];

	if (!referenceP.empty())
		referenceP.clear();

	float radius = 0.3f;
	constexpr float offset = PI_BEZIER / 4.0f;
	for (float angle = 0.0f; angle < PI_BEZIER * 2.0f; angle += offset) {
		referenceP.push_back(ref.x);
		referenceP.push_back(ref.y);
		referenceP.push_back(ref.z);

		referenceP.push_back(radius * std::cosf(angle) + ref.x);
		referenceP.push_back(radius * std::sinf(angle) + ref.y);
		referenceP.push_back(ref.z);

		referenceP.push_back(radius * std::cosf(angle + offset) + ref.x);
		referenceP.push_back(radius * std::sinf(angle + offset) + ref.y);
		referenceP.push_back(ref.z);
	}
	glutPostRedisplay();
}

void updateReference() {
	static glm::vec3 ref;
	int j = 0;
	ref.x = points[(size_t)i - 1];
	ref.y = points[i];
	ref.z = points[(size_t)i + 1];

	float radius = 0.3f;
	constexpr float offset = PI_BEZIER / 4.0f;
	for (float angle = 0.0f; angle < PI_BEZIER * 2.0f; angle += offset) {
		referenceP[j] = ref.x; j++;
		referenceP[j] = ref.y; j++;
		referenceP[j] = ref.z; j++;

		referenceP[j] = radius * std::cosf(angle) + ref.x; j++;
		referenceP[j] = radius * std::sinf(angle) + ref.y; j++;
		referenceP[j] = ref.z; j++;

		referenceP[j] = radius * std::cosf(angle + offset) + ref.x; j++;
		referenceP[j] = radius * std::sinf(angle + offset) + ref.y; j++;
		referenceP[j] = ref.z; j++;
	}
	j = 0;
	glBindBuffer(GL_ARRAY_BUFFER, referenceVBO);
	glBufferData(GL_ARRAY_BUFFER, referenceP.size() * sizeof(float), &referenceP[0], GL_DYNAMIC_DRAW);
}

glm::vec3 fromScreen2WorldCoords(int x, int y) {
	const float normalized_x = (2.0f * x / width) - 1.0f;
	const float normalized_y = 1.0f - (2.0f * y / height);

	glm::mat4 inverse_matrix_;

	inverse_matrix_ = inverse(projectionMatrix_bezier * viewMatrix_bezier);

	const glm::vec4 clip_point = inverse_matrix_ * glm::vec4(normalized_x, normalized_y, 0.0f, 1.0f);
	return glm::vec3(clip_point) / clip_point.w;
}

bool isClicked(glm::vec3 ctrlPt, glm::vec3 coords, int xcoef = 1, float alfa = 0.4f) {
	return
	xcoef* ctrlPt.x - alfa < coords.x
		&&
	coords.x < xcoef* ctrlPt.x + alfa
		&&
	ctrlPt.y - alfa < coords.y
		&&
	coords.y < ctrlPt.y + alfa;
}

bool inBoundsY(float y) {
	return y < maxY && y > minY;
}

bool inBoundsX(float x) {
	return x < maxX && x > minX;
}

void mouse_bezier(int button, int state, int x, int y) {
	if (button != GLUT_LEFT_BUTTON && state != GLUT_DOWN) return;

	glm::vec3 coords = fromScreen2WorldCoords(x,y);

	if (isClicked(pm3, coords * 100.0f)) selectedPoint = 0;
	else if (isClicked(pm2, coords * 100.0f)) selectedPoint = 1;
	else if (isClicked(pm1, coords * 100.0f)) selectedPoint = 2;
	else if (isClicked(p0, coords * 100.0f)) selectedPoint = 3;
	else if (isClicked(p1, coords * 100.0f)) selectedPoint = 4;
	else if (isClicked(p2, coords * 100.0f)) selectedPoint = 5;
	else if (isClicked(p3, coords * 100.0f)) selectedPoint = 6;
	else selectedPoint = -1;
}

void mouseMove_bezier(int x, int y) {
	glm::vec3 coords = fromScreen2WorldCoords(x, y) * 100.0f;
	switch (selectedPoint) {
	case 0:
		pm3.x = inBoundsX(coords.x) ? coords.x : pm3.x;
		pm3.y = inBoundsY(coords.y) ? coords.y : pm3.y;
		break;
	case 1:
		pm2.x = inBoundsX(coords.x) ? coords.x : pm2.x;
		pm2.y = inBoundsY(coords.y) ? coords.y : pm2.y;
		break;
	case 2:
		pm1.x = inBoundsX(coords.x) ? coords.x : pm1.x;
		pm1.y = inBoundsY(coords.y) ? coords.y : pm1.y;
		break;
	case 3:
		//p0.x = coords.x;
		p0.y = inBoundsY(coords.y) ? coords.y : p0.y;
		break;
	case 4:
		p1.x = inBoundsX(coords.x) ? coords.x : p1.x;
		p1.y = inBoundsY(coords.y) ? coords.y : p1.y;
		break;
	case 5:
		p2.x = inBoundsX(coords.x) ? coords.x : p2.x;
		p2.y = inBoundsY(coords.y) ? coords.y : p2.y;
		break;
	case 6:
		p3.x = inBoundsX(coords.x) ? coords.x : p3.x;
		p3.y = inBoundsY(coords.y) ? coords.y : p3.y;
		break;
	default:
		break;
	}
	refresh_bezier();
	refreshControlCircles();
	updateReference();
	refreshTangentLinePoints();
	glutPostRedisplay();
}
#include"ball.h"
#include"bezier.h"

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_SINGLE);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(700, 700);
	ball_wind = glutCreateWindow("Ball");

	init_ball();

	glutDisplayFunc(display_ball);
	glutReshapeFunc(reshape_ball);
	glutMouseFunc(mouse_ball);
	//2nd window
	bezier_wind = glutCreateWindow("Bezier");
	glutPositionWindow(900, 200);
	glutReshapeWindow(600,300);
	createPointsVector();
	createControlPointsVector();
	createReference();
	init_bezier();

	glutDisplayFunc(display_bezier);
	glutReshapeFunc(reshape_bezier);
	glutMouseFunc(mouse_bezier);
	glutMotionFunc(mouseMove_bezier);

	glutMainLoop();

	return 0;
}

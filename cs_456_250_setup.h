//----------------------------------------------------------------------------
void reshape_handler(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}	// end of reshape_handler()


//----------------------------------------------------------------------------
void init_setup(int width, int height, const char *windowName)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(5, 5);
	glutCreateWindow(windowName);
	glutReshapeFunc(reshape_handler);
}	// end of init_setup()


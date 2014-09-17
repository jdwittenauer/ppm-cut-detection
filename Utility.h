//----------------------------------------------------------------------------
// Store utility functions that are not related to the main algorithm here...
//----------------------------------------------------------------------------

#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <sstream>
using std::ostringstream;
using std::istringstream;
using std::string;
#include "Image.h"

//----------------------------------------------------------------------------
void DisplayText(int x, int y, string str, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glColor3f(r, g, b);
    for (size_t i = 0; i < str.length(); i++)
    {
        glRasterPos2i(x, y);
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
        x += 8; 
    }
}

//----------------------------------------------------------------------------
// Convert a type to string
//----------------------------------------------------------------------------
template <typename T>
string ToString(T num)
{
    ostringstream os;
    os << num;
    return os.str();
}

//----------------------------------------------------------------------------
void SetViewPort(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)width, 0.0, (GLdouble)height);
}

//----------------------------------------------------------------------------
int StringToInt(string str)
{
    istringstream instr(str);
    int n;
    instr >> n;
    return n;
}

//----------------------------------------------------------------------------
void ReadFrame(int frame, Image& image)
{
    char fileName[256];

    sprintf(fileName, "converted/project-video%.4db.ppm", frame);

    if (!ReadPPM(image, fileName))
    {
        printf("Something went wrong!\n");
        exit(0);
    }
}

//----------------------------------------------------------------------------
// Draw an image's pixels to screen.
//----------------------------------------------------------------------------
void DrawImage(Image& image)
{
    glBegin(GL_POINTS);
    for (int y = 0; y < IMAGE_HEIGHT; y++)
    {
        for (int x = 0; x < IMAGE_WIDTH; x++)
        {
            unsigned char* rgb = image.GetPixelAt(x, y);
            glColor3ub(rgb[0], rgb[1], rgb[2]);
            glVertex2i(x, IMAGE_HEIGHT - y);  // OpenGL interprets y=0 as the bottom (opposite of ppm y)
        }
    }
    glEnd();
}

#endif
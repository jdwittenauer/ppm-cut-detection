//-------------------------------------------------------------------
// CS 6250 Project 
// Shot Transition Detection
// Due Date: Nov 23, 2009
// Team Wolverine
// Group Member: John Wittenaur, Woon Tang, Mark Randles
//-------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <GL/glut.h>


const int IMAGE_WIDTH = 400;
const int IMAGE_HEIGHT = 300;
const int WINDOW_WIDTH = IMAGE_WIDTH * 2;
const int WINDOW_HEIGHT = IMAGE_HEIGHT * 2;
const char* WINDOW_TITLE = "PPM Video Cut Detection";
double FPS = 1000.0;  // frames per second

#include "WinTimer.h"
#include "Image.h"
#include "PPMReader.h"
#include "Utility.h"
#include "cs_456_250_setup.h"
using namespace std;

// These are the frame numbers of correct cuts
int CUTS[] = {191,286,385,429,461,532,580,677,732,795,1055,1104,1221,1299,1334,1413,1455,1487,1516,1591,1637,1678,1730,1780,1820,1853,1905,1932,1993,2028,2078,2254,2301,2321,2343,2354,2415,2520,2581,2626,2677,2723,2836,2892,2946,3008,3084,3126,3271,3314,3344,3407,3435,3474,3511,3524,3567,3587,3684,3797,3873,3913,3958,3990,4090,4484,4547,4608,4657,4707,4741,4794,4838,4864,4882,4897,4912,4930,4959,5010,5063,5437,5479,5522,5563,5595,5628,5657,5689,5733,5760,5796,5823,5860,5895,5930,5976,5990,5995,6007,6032,6047,6086,6111,6152,6185,6243,6311,6441,6479,6529,6586,6623,6673};
int DISSOLVES[] = {140,887,1055,2180,2986,4086,4446,5063,5391,6654};


int numSkipFrames = 1;

Image prevImg(IMAGE_WIDTH, IMAGE_HEIGHT);
Image currImg(IMAGE_WIDTH, IMAGE_HEIGHT);
WinTimer wt;
int currentFrameNum = 2;
const int lastFrameNum = 6783;

Image prevDiffImg(IMAGE_WIDTH, IMAGE_HEIGHT);
Image currDiffImg(IMAGE_WIDTH, IMAGE_HEIGHT);

int prevTotalDiff = 0;
bool isPromptUser = false;
string userInputText;

bool isDisplay = true;

bool inDissolve = false;
int dissolveStart = 0;
int dissolveEnd = 0;
int dissolveFrames = 0;

int totalDetectCorrectCut = 0;
int totalDetectFalseCut = 0;
int totalDetectCorrectDissolve = 0;
int totalDetectFalseDissolve = 0;

void keyboard(unsigned char key, int x, int y);
//----------------------------------------------------------------------------
// Waiting for user to input text.
//----------------------------------------------------------------------------
void PromptUser(unsigned char c, int x, int y)
{
	if (c == 13)	// if enter key is pressed
	{
        isPromptUser = false;
        currentFrameNum = StringToInt(userInputText);
        ReadFrame(currentFrameNum - 1, prevImg);
        ReadFrame(currentFrameNum, currImg);
        userInputText = "";
        glutKeyboardFunc(keyboard);
	}
	else if (c == 27)	// esc key is pressed. cancel operation
	{
		userInputText = "";
        isPromptUser = false;
		glutKeyboardFunc(keyboard);
	}
	else if (c == 8)	// backspace key pressed. erase last character.
		userInputText = userInputText.substr(0, userInputText.length() - 1);
	else
		userInputText += c;

	glutPostRedisplay();
}

//----------------------------------------------------------------------------
int helperFunc(int frameNum, int prevTotalDiff)
{
    int totalDiff = 0;
    Image currDiffImg2 = currDiffImg;
    Image prevDiffImg2 = currDiffImg2;
    ReadFrame(frameNum - 1, prevImg);
    ReadFrame(frameNum, currImg);

    // draw current frame
    for (int y = 0; y < IMAGE_HEIGHT; y++)
    {
        for (int x = 0; x < IMAGE_WIDTH; x++)
        {
            unsigned char* pRgb = prevImg.GetPixelAt(x, y);
            unsigned char* cRgb = currImg.GetPixelAt(x, y);
            unsigned char* cDiffRgb = currDiffImg2.GetPixelAt(x, y);
            cDiffRgb[0] = abs(cRgb[0] - pRgb[0]);
            cDiffRgb[1] = abs(cRgb[1] - pRgb[1]);
            cDiffRgb[2] = abs(cRgb[2] - pRgb[2]);

            unsigned char* prevDiffRgb = prevDiffImg2.GetPixelAt(x, y);

            totalDiff += prevDiffRgb[0] + prevDiffRgb[1] + prevDiffRgb[2];
        }
    }

    int beforeTotalDiff = abs(totalDiff - prevTotalDiff);
    int prevTotalNextDiff = totalDiff;


    totalDiff = 0;
    for (int y = 0; y < IMAGE_HEIGHT; y++)
    {
        for (int x = 0; x < IMAGE_WIDTH; x++)
        {
            unsigned char* diffRgb = currDiffImg2.GetPixelAt(x, y);

            totalDiff += diffRgb[0] + diffRgb[1] + diffRgb[2];
        }
    }

    if (totalDiff == 0)
    {
        return helperFunc(frameNum + 1, prevTotalNextDiff);
    }
    else 
    {
        if (prevTotalNextDiff == 0)
            return abs(totalDiff - beforeTotalDiff);
        else
            return abs(totalDiff - prevTotalNextDiff);
    }
}

void DisplayStatistics()
{
    wt.Stop();

    int totalMissCut = (sizeof(CUTS) / sizeof(int)) - totalDetectCorrectCut;
    int totalMissDissolve = (sizeof(DISSOLVES) / sizeof(int)) - totalDetectCorrectDissolve;

    ofstream fout("log.txt", ios::app);
    fout << "------------------------------------------------------------\n";
    fout << "Total correct cuts detected: " << totalDetectCorrectCut << "\n";
    fout << "Total false cuts detected: " << totalDetectFalseCut << "\n";
    fout << "Total correct cuts missed: " << totalMissCut << "\n\n";
    fout << "------------------------------------------------------------\n";
    fout << "Total correct dissolves detected: " << totalDetectCorrectDissolve << "\n";
    fout << "Total false dissolves detected: " << totalDetectFalseDissolve << "\n";
    fout << "Total correct dissolves missed: " << totalMissCut << "\n\n";
    fout << "------------------------------------------------------------\n";
    fout << "Total time elapsed: " << wt.GetElapsedSeconds() << " seconds\n\n";
    fout.close();

    cout << "------------------------------------------------------------\n";
    cout << "Total correct cuts detected: " << totalDetectCorrectCut << "\n";
    cout << "Total false cuts detected: " << totalDetectFalseCut << "\n";
    cout << "Total correct cuts missed: " << totalMissCut << "\n\n";
    cout << "------------------------------------------------------------\n";
    cout << "Total correct dissolves detected: " << totalDetectCorrectDissolve << "\n";
    cout << "Total false dissolves detected: " << totalDetectFalseDissolve << "\n";
    cout << "Total correct dissolves missed: " << totalMissDissolve << "\n\n";
    cout << "------------------------------------------------------------\n";
    cout << "Total time elapsed: " << wt.GetElapsedSeconds() << " seconds\n\n";
}

//----------------------------------------------------------------------------
void display_func()
{
    if (currentFrameNum >= lastFrameNum)
    {
        DisplayStatistics();
        exit(0);
    }

    if (isPromptUser)
    {
        SetViewPort(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        // draw a black rectangle to clear previous entered text
        glColor3f(0.0f, 0.0f, 0.0f);
        glRecti(50, WINDOW_HEIGHT - 40, 300, WINDOW_HEIGHT - 50);

        DisplayText(50, WINDOW_HEIGHT - 50, "Goto frame: " + userInputText, 1.0f, 1.0f, 1.0f);
        glutSwapBuffers();

        return;
    }

    if (isDisplay)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        //------------------------------------------------------------------------
        // Upper left view port (Displays previous frame)
        //------------------------------------------------------------------------
        SetViewPort(0, IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT);

        DrawImage(prevImg);
    
        // Display previous frame number
        DisplayText(10, IMAGE_HEIGHT - 15, "Prev frame: " + ToString(currentFrameNum - 1), 1.0f, 1.0f, 1.0f);
        DisplayText(10, IMAGE_HEIGHT - 30, "FPS: " + ToString(FPS), 1.0f, 1.0f, 1.0f);
        DisplayText(10, IMAGE_HEIGHT - 45, "Speed: " + ToString(numSkipFrames) + "x", 1.0f, 1.0f, 1.0f);
    }

    //------------------------------------------------------------------------
    // Upper right view port (Displays current frame)
    //------------------------------------------------------------------------
    
    // Start histogram analysis between previous and current frame
    int hist1[64] = {0};
    int hist2[64] = {0};
    for (int y = 0; y < IMAGE_HEIGHT; y++)
    {
        for (int x = 0; x < IMAGE_WIDTH; x++)
        {
            unsigned char* prevRgb = prevImg.GetPixelAt(x, y);
            unsigned char* currRgb = currImg.GetPixelAt(x, y);
            hist1[((prevRgb[0] & 0xC0) >> 2) | ((prevRgb[1] & 0xC0) >> 4) | ((prevRgb[2] & 0xC0) >> 6)]++;
            hist2[((currRgb[0] & 0xC0) >> 2) | ((currRgb[1] & 0xC0) >> 4) | ((currRgb[2] & 0xC0) >> 6)]++;
        }
    }

    int sumHistogramDifference = 0;
    for (int i = 0; i < 64; i++)
    {
        sumHistogramDifference += abs(hist2[i] - hist1[i]);
    }

    if (isDisplay)
    {
        SetViewPort(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT);

        DrawImage(currImg);
        // Display current frame number
        DisplayText(10, IMAGE_HEIGHT - 15, "Curr frame: " + ToString(currentFrameNum), 1.0f, 1.0f, 1.0f);
        DisplayText(10, IMAGE_HEIGHT - 30, "Sum of histogram diff: " + ToString(sumHistogramDifference), 1.0f, 1.0f, 1.0f);
    }
    
    // Is the previous and current frame different?
    bool isDifferentFrame = (sumHistogramDifference == 0) ? false : true;


    //------------------------------------------------------------------------
    // Lower left view port (Displays diff of prev frame and prev-prev frame)
    //------------------------------------------------------------------------
    
    int totalDiff = 0;
    prevDiffImg = currDiffImg;
    
    currDiffImg = currImg - prevImg;

    int beforeTotalDiff = 0;
    if (isDisplay)
    {
        SetViewPort(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);

        // draw current frame
        glBegin(GL_POINTS);
        for (int y = 0; y < IMAGE_HEIGHT; y++)
        {
            for (int x = 0; x < IMAGE_WIDTH; x++)
            {
                unsigned char* prevDiffRgb = prevDiffImg.GetPixelAt(x, y);

                totalDiff += prevDiffRgb[0] + prevDiffRgb[1] + prevDiffRgb[2];

                glColor3ub(prevDiffRgb[0], prevDiffRgb[1], prevDiffRgb[2]);
                glVertex2i(x, IMAGE_HEIGHT - y);
            }
        }
        glEnd();

        beforeTotalDiff = abs(totalDiff - prevTotalDiff);
        prevTotalDiff = totalDiff;

        DisplayText(10, IMAGE_HEIGHT - 10, "Diff btw prev and prev x2", 1.0f, 1.0f, 1.0f);
    }
    else
    {
        for (int y = 0; y < IMAGE_HEIGHT; y++)
        {
            for (int x = 0; x < IMAGE_WIDTH; x++)
            {
                unsigned char* prevDiffRgb = prevDiffImg.GetPixelAt(x, y);

                totalDiff += prevDiffRgb[0] + prevDiffRgb[1] + prevDiffRgb[2];
            }
        }

        beforeTotalDiff = abs(totalDiff - prevTotalDiff);
        prevTotalDiff = totalDiff;
    }

   
    //------------------------------------------------------------------------
    // Lower right viewport (Displays diff of current and prev frame)
    //------------------------------------------------------------------------

    totalDiff = 0;

    if (isDisplay)
    {
        SetViewPort(IMAGE_WIDTH, 0, IMAGE_WIDTH, IMAGE_HEIGHT);

        // draw current frame
        glBegin(GL_POINTS);
        for (int y = 0; y < IMAGE_HEIGHT; y++)
        {
            for (int x = 0; x < IMAGE_WIDTH; x++)
            {
                unsigned char* diffRgb = currDiffImg.GetPixelAt(x, y);

                totalDiff += diffRgb[0] + diffRgb[1] + diffRgb[2];
                glColor3ub(diffRgb[0], diffRgb[1], diffRgb[2]);
                glVertex2i(x, IMAGE_HEIGHT - y);
            }
        }
        glEnd();
    }
    else
    {
        for (int y = 0; y < IMAGE_HEIGHT; y++)
        {
            for (int x = 0; x < IMAGE_WIDTH; x++)
            {
                unsigned char* diffRgb = currDiffImg.GetPixelAt(x, y);

                totalDiff += diffRgb[0] + diffRgb[1] + diffRgb[2];
            }
        }
    }

    int afterTotalDiff;
    if (prevTotalDiff == 0)
    {
        afterTotalDiff = abs(totalDiff - beforeTotalDiff);
        prevTotalDiff = beforeTotalDiff;
    }
    else
        afterTotalDiff = abs(totalDiff - prevTotalDiff);

    int compare1 = afterTotalDiff;
    int compare2 = 0;

    if (isDisplay)
        DisplayText(10, IMAGE_HEIGHT - 10, "Diff btw curr and prev", 1.0f, 1.0f, 1.0f);

    if (isDifferentFrame)
    {
        // call to recursive function
        compare2 = helperFunc(currentFrameNum + 1, prevTotalDiff);

        if (isDisplay)
        {
            DisplayText(10, IMAGE_HEIGHT - 25, "Prev 2nd order diff: " + ToString(compare1), 1.0f, 1.0f, 1.0f);
            DisplayText(10, IMAGE_HEIGHT - 40, "Next 2nd order diff: " + ToString(compare2), 1.0f, 1.0f, 1.0f);
        }
    }
    else
    {
        if (isDisplay)
        {
            DisplayText(10, IMAGE_HEIGHT - 25, "Prev 2nd order diff: N/A", 1.0f, 1.0f, 1.0f);
            DisplayText(10, IMAGE_HEIGHT - 40, "Next 2nd order diff: N/A", 1.0f, 1.0f, 1.0f);
        }
    }


    bool correctCut = false;
    if (sumHistogramDifference >= 1800 && compare1 >= 1022000 && compare2 >= 1022000) {
        ofstream fout("log.txt", ios::app);
        int ttsz = sizeof(CUTS) / sizeof(int);
        for (int k = 0; k < ttsz; k++)
        {
            if (currentFrameNum == CUTS[k])
            {
                correctCut = true;
                break;
            }
        }

        if (correctCut)
        {
            totalDetectCorrectCut++;

            fout << "---------------------------------------\n";
            fout << "Detected correct cut at frame " << currentFrameNum << "!\n";
            fout << "---------------------------------------\n";
            fout << "Sum of histogram difference = " << sumHistogramDifference << endl;
            fout << "Previous second order difference = " << compare1 << "\n";
            fout << "Next second order difference = " << compare2 << "\n\n";

            cout << "---------------------------------------\n";
            cout << "Detected correct cut at frame " << currentFrameNum << "!\n";
            cout << "---------------------------------------\n";
            cout << "Sum of histogram difference = " << sumHistogramDifference << endl;
            cout << "Previous second order difference = " << compare1 << "\n";
            cout << "Next second order difference = " << compare2 << "\n\n";
        }
        else
        {
            totalDetectFalseCut++;

            fout << "***************************************\n";
            fout << "Detected false cut at frame " << currentFrameNum << "...\n";
            fout << "***************************************\n";
            fout << "Sum of histogram difference = " << sumHistogramDifference << endl;
            fout << "Previous second order difference = " << compare1 << "\n";
            fout << "Next second order difference = " << compare2 << "\n\n";

            cout << "***************************************\n";
            cout << "Detected false cut at frame " << currentFrameNum << "...\n";
            cout << "***************************************\n";
            cout << "Sum of histogram difference = " << sumHistogramDifference << endl;
            cout << "Previous second order difference = " << compare1 << "\n";
            cout << "Next second order difference = " << compare2 << "\n\n";

        }
        fout.close();
    }

	// Dissolve detection
	bool correctDissolve = false;
	if (sumHistogramDifference >= 1800 && compare1 >= 20000 && !inDissolve) 
	{
		inDissolve = true;
		dissolveStart = currentFrameNum;
		dissolveEnd = currentFrameNum + 20;
		dissolveFrames = 1;
	}
	else if (inDissolve) 
	{
		if (sumHistogramDifference >= 1200 && compare1 >= 20000) 
		{
			dissolveFrames++;
		}

		if (dissolveFrames >= 15) 
		{
			//ofstream fout("hist-perfect-new.txt", ios::app);
			int ttsz = sizeof(DISSOLVES) / sizeof(int);

			for (int k = 0; k < ttsz; k++)
			{
				if (dissolveStart == DISSOLVES[k])
				{
					correctDissolve = true;
					break;
				}
			}

            ofstream fout("log.txt", ios::app);
			if (correctDissolve)
			{
                totalDetectCorrectDissolve++;

				fout << "---------------------------------------------------\n";
                fout << "Detected correct dissolve starting at frame " << dissolveStart << "!\n";
                fout << "---------------------------------------------------\n";
                fout << "Sum of histogram difference = " << sumHistogramDifference << endl;
                fout << "Previous second order difference = " << compare1 << "\n";
                fout << "Next second order difference = " << compare2 << "\n\n";

				cout << "---------------------------------------------------\n";
                cout << "Detected correct dissolve starting at frame " << dissolveStart << "!\n";
                cout << "---------------------------------------------------\n";
                cout << "Sum of histogram difference = " << sumHistogramDifference << endl;
                cout << "Previous second order difference = " << compare1 << "\n";
                cout << "Next second order difference = " << compare2 << "\n\n";
			}
			else
			{
                totalDetectFalseDissolve++;

				fout << "***************************************************\n";
                fout << "Detected false dissolve starting at frame " << dissolveStart << "...\n";
                fout << "***************************************************\n";
                fout << "Sum of histogram difference = " << sumHistogramDifference << endl;
                fout << "Previous second order difference = " << compare1 << "\n";
                fout << "Next second order difference = " << compare2 << "\n\n";

				cout << "***************************************************\n";
                cout << "Detected false dissolve starting at frame " << dissolveStart << "...\n";
                cout << "***************************************************\n";
                cout << "Sum of histogram difference = " << sumHistogramDifference << endl;
                cout << "Previous second order difference = " << compare1 << "\n";
                cout << "Next second order difference = " << compare2 << "\n\n";
			}
            fout.close();

			inDissolve = false;
		}
		else if (currentFrameNum > dissolveEnd) 
		{
			inDissolve = false;
		}
	}

    if (isDisplay)
    {
        currentFrameNum += numSkipFrames;
        glutSwapBuffers();
    }
    else
    {
        currentFrameNum++;
    }
}

//----------------------------------------------------------------------------
void refreshFrame(int val)
{    
    if (numSkipFrames != 0)
    {
        ReadFrame(currentFrameNum - 1, prevImg);
        ReadFrame(currentFrameNum, currImg);

        glutPostRedisplay();
    }

    glutTimerFunc(1000.0 / FPS, refreshFrame, 0);
}

//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'g':
        isPromptUser = true;
        glutKeyboardFunc(PromptUser);
        glutPostRedisplay();
        break;
    case 'p':
        numSkipFrames = (numSkipFrames == 0) ? 1 : 0;

        glutPostRedisplay();
        break;
    case ']':
        if (FPS <= 50.01)
        {
            if (FPS >= 49.99)
                FPS = 1000.0;
            else if (FPS >= 29.99)
                FPS = 50.0;
            else 
                FPS = 30.0;
        }
        glutPostRedisplay();
        break;
    case '[':
        if (FPS >= 10.01)
        {
            if (FPS <= 30.01)
                FPS = 10.0;
            else if (FPS <= 50.01)
                FPS = 30.0;
            else 
                FPS = 50.0;
        }
        glutPostRedisplay();
        break;
    case '.':
        if (numSkipFrames >= 1)
            numSkipFrames *= 2;
        else if (numSkipFrames == -1)
            numSkipFrames = 1;
        else
            numSkipFrames /= 2;

        glutPostRedisplay();
        break;
    case ',':
        if (numSkipFrames > 1)
            numSkipFrames /= 2;
        else if (numSkipFrames == 1)
            numSkipFrames = -1;
        else
            numSkipFrames *= 2;

        glutPostRedisplay();
        break;
    case 'a':
        numSkipFrames = 0;
        currentFrameNum--;
        ReadFrame(currentFrameNum - 1, prevImg);
        ReadFrame(currentFrameNum, currImg);
        glutPostRedisplay();
        break;
    case 's':
        numSkipFrames = 0;
        currentFrameNum++;
        ReadFrame(currentFrameNum - 1, prevImg);
        ReadFrame(currentFrameNum, currImg);
        glutPostRedisplay();
        break;
    case 'd':
        if (isDisplay)
        {
            isDisplay = false;
            glClear(GL_COLOR_BUFFER_BIT);
            glutSwapBuffers();
        }
        else
        {
            isDisplay = true;
       }
        glutPostRedisplay();
        break;
    case 'q':
        exit(0);
    }
}

void DisplayHelp()
{
    cout << "Instructions\n";
    cout << "------------\n";
    cout << "'p' - Toggle play / pause\n";
    cout << "']' - Increase fps\n";
    cout << "'[' - Decrease fps\n";
    cout << "'.' - Fast forward x2\n";
    cout << "',' - Rewind x2\n";
    cout << "'s' - Step forward one frame\n";
    cout << "'a' - Step back one frame\n";
    cout << "'g' - Goto frame #\n";
    cout << "'d' - Toggle display on / off\n";
    cout << "'q' - Quit program\n";
}

//----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);

    init_setup(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    glutDisplayFunc(display_func);
    glutKeyboardFunc(keyboard);

    glutTimerFunc(1000.0 / FPS, refreshFrame, 0);

    DisplayHelp();

    // create a new log file
    ofstream fout("log.txt");
    cout << "Creating a new log file 'log.txt'...\n\n";
    fout.close();

    // Start Timer
    wt.Start();

    glutMainLoop();

}
#ifndef IMAGE_H
#define IMAGE_H

#include <cassert>
#include <cmath>

//----------------------------------------------------------------------------
// Image structure.
//----------------------------------------------------------------------------
struct Image
{
    Image();
    Image(int w, int h);
    Image(const Image& rhs);
    ~Image();
    const Image& operator = (const Image& rhs);
    Image operator - (const Image& rhs);

    bool IsValidPixel(int x, int y);
    unsigned char* GetPixelAt(int x, int y);

    int width, height;
    unsigned char* buf;
    int bufSize;    // for internal use (stores the buffer's size)
};

//----------------------------------------------------------------------------
Image::Image()
{
    width = 0;
    height = 0;
    buf = NULL;
    bufSize = 0;
}

//----------------------------------------------------------------------------
Image::Image(int w, int h)
{
    width = w;
    height = h;
    bufSize = 3 * w * h;
    buf = new unsigned char[bufSize];

    memset(buf, 0, bufSize);
}

//----------------------------------------------------------------------------
Image::Image(const Image& rhs)
{
    width = rhs.width;
    height = rhs.height;
    
    bufSize = 3 * rhs.width * rhs.height;
    buf = new unsigned char[bufSize];

    memcpy(buf, rhs.buf, bufSize);
}

//----------------------------------------------------------------------------
Image::~Image()
{
    delete[] buf;
}

//----------------------------------------------------------------------------
// Handles self assignment (implicit) and is exception safe. 
// See this for more details:
// http://www.parashift.com/c++-faq-lite/assignment-operators.html#faq-12.3
//----------------------------------------------------------------------------
const Image& Image::operator = (const Image& rhs)
{
    unsigned char* origBuf = buf;
    
    buf = new unsigned char[3 * rhs.width * rhs.height];

    memcpy(buf, rhs.buf, bufSize);

    delete[] origBuf;

    width = rhs.width;
    height = rhs.height;
    bufSize = 3 * rhs.width * rhs.height;

    return *this;
}

//----------------------------------------------------------------------------
// Returns an image where each of its color component is the absolute 
// difference of image A and image B's color.
//----------------------------------------------------------------------------
Image Image::operator - (const Image& rhs)
{
    // Two frames must be of same size to be compared.
    assert(bufSize == rhs.bufSize);

    Image imgDiff(width, height);

    for (int i = 0; i < bufSize; i++)
    {
        imgDiff.buf[i] = abs(this->buf[i] - rhs.buf[i]);
    }

    return imgDiff;
}

//----------------------------------------------------------------------------
bool Image::IsValidPixel(int x, int y)
{
    if (x < 0 || x > width - 1 || y < 0 || y > height - 1)
        return false;

    return true;
}

//----------------------------------------------------------------------------
// Get the pointer p to the red color component of pixel at (x,y).
// Note: p[0], p[1], p[2] are r,g,b components of pixel at location (x,y).
//----------------------------------------------------------------------------
unsigned char* Image::GetPixelAt(int x, int y)
{
    // index checking... Remove this in release version for efficiency
    /*if (!IsValidPixel(x, y))
    {
        fprintf(stderr, "x: %d y: %d -> Invalid index\n", x, y);
        exit(0);
    }*/

    int i = 3 * (x + y * width);
    
    return &buf[i];
}

#endif
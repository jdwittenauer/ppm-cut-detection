#ifndef PPM_READER_H
#define PPM_READER_H

//----------------------------------------------------------------------------
// Reads the magic number from a PPM file. 
//
// @param fileName
//      The file name of the PPM file you want to open.
// @return 
//      The magic number (from 1 to 6) if success.
//      -1 if error occurs.
//----------------------------------------------------------------------------
int ReadMagicNumber(char* fileName)
{
    // Opens as a text file. The first line of PPM file is always text-based.
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        perror(fileName);
        return -1;
    }

    char buf[256];

    // Reads the first line of file. Note that fgets() gets the entire line 
    // including the '\n' character at the end.
    char* t = fgets(buf, 256, fp);
    if (t == NULL)
    {
        fprintf(stderr, "Error reading magic number\n");
        return -1;
    }

    // Length of first line must be 3 (Including the '\n' character!). The 
    // first character always starts with a P.
    if (strlen(buf) != 3 || buf[0] != 'P')
    {
        fprintf(stderr, "%s: Not a PPM file\n", fileName);
        return -1;
    }
    // Magic number is from P1 to P6.
    if (buf[1] < '1' || buf[1] > '6')
    {
        fprintf(stderr, "Invalid magic number: %c\n", buf[1]);
        return -1;
    }

    fclose(fp);

    return buf[1] - '0';
}


//----------------------------------------------------------------------------
// Reads the PPM file in binary format.
//
// @param image
//      The image object you want the data to be written to.
// @param fileName
//      The file name of the PPM file you want to open.
// @return 
//      Whether PPM file is read correctly.
//----------------------------------------------------------------------------
bool ReadPPMB(Image& image, char* fileName)
{
    FILE* fp = fopen(fileName, "rb");
    if (fp == NULL)
    {
        perror(fileName);
        return false;
    }
    
    char buf[256];
    // Get rid of the first line (magic number)
    fgets(buf, 256, fp);

    // Get rid of comments (can be 0, 1 or more lines)
    char* t;
    do
    {
        t = fgets(buf, 256, fp);
        if (t == NULL) 
        {
            fprintf(stderr, "Error reading header\n");
            return false;
        }
    }
    while (buf[0] == '#');

    // Read width and height
    unsigned int w, h;
    int n = sscanf(buf, "%u %u", &w, &h);
    if (n != 2)
    {
        fprintf(stderr, "Error reading width and height\n");
        return false;
    }
    
    int maxVal;
    // Read maximum color component
    n = fscanf(fp, "%u\n", &maxVal);
    if (n != 1)
    {
        fprintf(stderr, "Error reading maximum color component\n");
        return false;
    }

    // Each color component must fit in 1 byte
    if (maxVal != 255)
    {
        fprintf(stderr, "Maximum color component = %d not supported.\n", maxVal);
        fprintf(stderr, "Only maximum of 255 is supported.\n");
        return false;
    }

    image = Image(w, h);

    // Read the entire RGB values into the image buffer
    size_t readCount = fread(image.buf, 1, 3 * w * h, fp);
    if (readCount != 3 * w * h)
    {
        fprintf(stderr, "Error reading RGB values\n");
        return false;
    }
    
    fclose(fp);

    return true;
}

//----------------------------------------------------------------------------
// Reads the PPM file in ascii format.
//
// @param image
//      The image object you want the data to be written to.
// @param fileName
//      The file name of the PPM file you want to open.
// @return 
//      Whether PPM file is read correctly.
//----------------------------------------------------------------------------
bool ReadPPMA(Image& image, char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        perror(fileName);
        return false;
    }
    
    char buf[256];
    // Get rid of the first line (magic number)
    fgets(buf, 256, fp);

    // Get rid of comments (can be 0, 1 or more lines)
    char* t;
    do
    {
        t = fgets(buf, 256, fp);
        if (t == NULL) 
        {
            fprintf(stderr, "Error reading header\n");
            return false;
        }
    }
    while (buf[0] == '#');

    // Read width and height
    unsigned int w, h;
    int n = sscanf(buf, "%u %u", &w, &h);
    if (n != 2)
    {
        fprintf(stderr, "Error reading width and height\n");
        return false;
    }
    
    int maxVal;
    // Read maximum color component
    n = fscanf(fp, "%u\n", &maxVal);
    if (n != 1)
    {
        fprintf(stderr, "Error reading maximum color component\n");
        return false;
    }

    // Each color component must fit in 1 byte
    if (maxVal != 255)
    {
        fprintf(stderr, "Maximum color component = %d not supported.\n", maxVal);
        fprintf(stderr, "Only maximum of 255 is supported.\n");
        return false;
    }

    image = Image(w, h);

    unsigned int r, g, b;
    int totalPixels = w * h;
    // Read the RGB values into the image buffer
    for (int i = 0; i < totalPixels; i++)
    {
        n = fscanf(fp, "%u %u %u", &r, &g, &b);
        
        //cout << "("<<x<<","<<y<<")   "<< "r: " << (int)red << "  g: " << (int)green << "  b: " << (int)blue<< endl;
        if (n != 3)
        {
            fprintf(stderr, "Error reading RGB values\n");
            return false;
        }

        image.buf[3 * i]     = r;
        image.buf[3 * i + 1] = g;
        image.buf[3 * i + 2] = b;
    }
    
    fclose(fp);

    return true;
}

//----------------------------------------------------------------------------
// Reads a PPM file, determines the type (either text or binary) and opens it
// accordingly.
//
// @param image
//      The image object you want the data to be written to.
// @param fileName
//      The file name of the PPM file you want to open.
// @return 
//      Whether PPM file is read correctly.
//----------------------------------------------------------------------------
bool ReadPPM(Image& image, char* fileName)
{
    int magicNumber = ReadMagicNumber(fileName);
    // Read in ascii mode
    if (magicNumber == 3)
    {
        return ReadPPMA(image, fileName);
    }
    // Read in binary mode
    else if (magicNumber == 6)
    {
        // Close and re-open in binary mode
        return ReadPPMB(image, fileName);
    }
    // Error occured in readMagicNumber
    else if (magicNumber == -1)
    {
        return false;
    }
    // Valid magic number, but not supported
    else
    {
        fprintf(stderr, "Sorry, magic number P%d not supported\n", magicNumber);
        return false;
    }
}

#endif
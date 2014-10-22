#include "correction.h"

Correction::Correction(PNM* img) :
    Transformation(img)
{
}

Correction::Correction(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* Correction::transform()
{
    float shift  = getParameter("shift").toFloat();
    float factor = getParameter("factor").toFloat();
    float gamma  = getParameter("gamma").toFloat();

    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    for (int i=0; i < PIXEL_VAL_MAX+1; i++)
    {
        LUT[i] = i;
        LUT[i] = LUT[i] + shift;
        LUT[i] = LUT[i] * factor;
        LUT[i] = pow(LUT[i], gamma);

        if (LUT[i] < 0)
        {
            LUT[i] = 0;
        }
        if (LUT[i] > 255)
        {
            LUT[i] = 255;
        }
    }

    // Iterate over image space
    for (int x=0; x<width; x++)
        for (int y=0; y<height; y++)
        {
            QRgb pixel = image->pixel(x,y); // Getting the pixel(x,y) value

            int r = qRed(pixel);    // Get the 0-255 value of the R channel
            int g = qGreen(pixel);  // Get the 0-255 value of the G channel
            int b = qBlue(pixel);   // Get the 0-255 value of the B channel
            r = LUT[r];
            g = LUT[g];
            b = LUT[b];
            QColor newPixel = QColor(r,g,b);
            newImage->setPixel(x,y,newPixel.rgb());
        }


    return newImage;
}

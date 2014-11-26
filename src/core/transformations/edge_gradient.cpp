#include "edge_gradient.h"

EdgeGradient::EdgeGradient(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

EdgeGradient::EdgeGradient(PNM* img) :
    Convolution(img)
{
}

PNM* EdgeGradient::verticalDetection()
{
    return convolute(g_y, RepeatEdge);
}

PNM* EdgeGradient::horizontalDetection()
{
    return convolute(g_x, RepeatEdge);
}

PNM* EdgeGradient::transform()
{

    PNM* newImage = new PNM(image->width(), image->height(), image->format());

    int width = image->width();
    int height = image->height();

    PNM* xImage = horizontalDetection();
    PNM* yImage = verticalDetection();

    if (image->format() == QImage::Format_Indexed8)
    {
        for (int i=0; i<width; i++)
        {
            for (int j=0; j<height; j++)
            {
                int xPix = qGray(xImage->pixel(i,j));
                int yPix = qGray(yImage->pixel(i,j));
                int newPix = sqrt(xPix*xPix + yPix*yPix);
                newImage->setPixel(i,j,newPix);
            }
        }
    }
    else
    {
        for (int i=0; i<width; i++)
        {
            for (int j=0; j<height; j++)
            {
                int rX = qRed(xImage->pixel(i,j));
                int rY = qRed(yImage->pixel(i,j));

                int gX = qGreen(xImage->pixel(i,j));
                int gY = qGreen(yImage->pixel(i,j));

                int bX = qBlue(xImage->pixel(i,j));
                int bY = qBlue(yImage->pixel(i,j));

                int r = sqrt(rX*rX + rY*rY);
                int g = sqrt(gX*gX + gY*gY);
                int b = sqrt(bX*bX + bY*bY);

                newImage->setPixel(i,j,qRgb(r,g,b));
            }
        }

    }


    return newImage;
}


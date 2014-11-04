#include "bin_manual.h"

BinarizationManual::BinarizationManual(PNM* img) :
    Transformation(img)
{
}

BinarizationManual::BinarizationManual(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationManual::transform()
{
    int threshold = getParameter("threshold").toInt();

    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){

            QRgb pixel = image->pixel(i,j);
            int t = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3;

            if ( t > threshold ){
                newImage->setPixel(i, j, Qt::color1);
            }
            else {
                newImage->setPixel(i, j, Qt::color0);
            }
        }
    }

    return newImage;
}





#include "bin_gradient.h"

BinarizationGradient::BinarizationGradient(PNM* img) :
    Transformation(img)
{
}

BinarizationGradient::BinarizationGradient(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationGradient::transform()
{
    int width = image->width();
    int height = image->height();
    int Gmax = 0;
    int a = 0;
    int b = 0;
    int newT = 0;

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    for(int x=1; x<width-1; x++){
        for(int y=1; y<height-1; y++){

            QRgb pixel = image->pixel(x,y);
            QRgb pixel1a = image->pixel(x+1,y);
            QRgb pixel1b = image->pixel(x-1,y);
            QRgb pixel2a = image->pixel(x,y+1);
            QRgb pixel2b = image->pixel(x,y-1);
            int t = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3;
            int Gxa = (qRed(pixel1a) + qGreen(pixel1a) + qBlue(pixel1a)) / 3;
            int Gxb = (qRed(pixel1b) + qGreen(pixel1b) + qBlue(pixel1b)) / 3;
            int Gya = (qRed(pixel2a) + qGreen(pixel2a) + qBlue(pixel2a)) / 3;
            int Gyb= (qRed(pixel2b) + qGreen(pixel2b) + qBlue(pixel2b)) / 3;


            int Gx = Gxa - Gxb;
            int Gy = Gya - Gyb;

            if (Gx > Gy) Gmax = Gx;
            else Gmax = Gy;

            a += t * Gmax;
            b += Gmax;

        }
    }

    newT = a/b;

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){

            QRgb pixel = image->pixel(i,j);
            int t = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3;

            if ( t > newT ){
                newImage->setPixel(i, j, Qt::color1);
            }
            else {
                newImage->setPixel(i, j, Qt::color0);
            }
        }
    }

    return newImage;
}



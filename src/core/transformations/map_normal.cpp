#include "map_normal.h"

#include "edge_sobel.h"
#include "map_height.h"

MapNormal::MapNormal(PNM* img) :
    Convolution(img)
{
}

MapNormal::MapNormal(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* MapNormal::transform()
{
    int width  = image->width(),
        height = image->height();

    double strength = getParameter("strength").toDouble();

    PNM* newImage = new PNM(width, height, image->format());
    PNM* tempImage = MapHeight(image).transform();

    EdgeSobel sobel(tempImage);
    math::matrix<float>* Gx = sobel.rawHorizontalDetection();
    math::matrix<float>* Gy = sobel.rawVerticalDetection();

    newImage = new PNM(width, height, QImage::Format_RGB32);

    for(int i=0; i<width; i++)
    {
         for(int j=0; j<height; j++)
         {
             double dx = (*Gx)(i,j)/PIXEL_VAL_MAX;
             double dy = (*Gy)(i,j)/PIXEL_VAL_MAX;
             double dz = 1/strength;

             double length = sqrt(dx*dx + dy*dy + dz*dz);
             dx = dx/length;
             dy = dy/length;
             double dZ = dz/length;

             double t = 255/strength;
             dx = (dx + 1.0)* t;
             dy = (dy + 1.0)* t;
             dZ = (dZ + 1.0)* t;

             newImage->setPixel(i,j,qRgb(dx,dy,dZ));
         }
     }

    return newImage;
}

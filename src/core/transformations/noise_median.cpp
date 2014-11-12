#include "noise_median.h"

NoiseMedian::NoiseMedian(PNM* img) :
    Convolution(img)
{
}

NoiseMedian::NoiseMedian(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* NoiseMedian::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    if(image->format() == QImage::Format_Indexed8)
    {
       for(int x=0; x<width; x++)
           for(int y=0; y<height; y++)
           {
               int l = getMedian(x, y, LChannel);
               newImage->setPixel(x, y, l);
           }
    }
    else {
       for(int x=0; x<width; x++)
           for(int y=0; y<height; y++)
           {
              int r = getMedian(x, y, RChannel);
              int g = getMedian(x, y, GChannel);
              int b = getMedian(x, y, BChannel);

              QColor newPixel = QColor(r,g,b);
              newImage->setPixel(x, y, newPixel.rgb());
           }
       }
    return newImage;
}

int NoiseMedian::getMedian(int x, int y, Channel channel)
{
    int radius = getParameter("radius").toInt();

    math::matrix<float> window = getWindow(x, y, (radius*2)+1, channel, RepeatEdge);

    int size = window.rowno();
    QVector<float> vector;

    for (int x=0; x<size; x++)
         for (int y=0; y<size; y++)
         {
           vector.append(window(x, y));
         }

    qSort(vector);
    float median = vector.at(vector.size()/2);

    return median;
}

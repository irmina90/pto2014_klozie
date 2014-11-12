#include "noise_bilateral.h"

NoiseBilateral::NoiseBilateral(PNM* img) :
    Convolution(img)
{
}

NoiseBilateral::NoiseBilateral(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* NoiseBilateral::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    sigma_d = getParameter("sigma_d").toInt();
    sigma_r = getParameter("sigma_r").toInt();
    radius = sigma_d;

    if(image->format() == QImage::Format_Indexed8)
    {
       for(int x=0; x<width; x++)
           for(int y=0; y<height; y++)
           {
               int l = calcVal(x, y, LChannel);
               newImage->setPixel(x, y, l);
           }
    }
    else {
       for(int x=0; x<width; x++)
           for(int y=0; y<height; y++)
           {
              int r = calcVal(x, y, RChannel);
              int g = calcVal(x, y, GChannel);
              int b = calcVal(x, y, BChannel);

              QColor newPixel = QColor(r,g,b);
              newImage->setPixel(x, y, newPixel.rgb());
           }
       }

    return newImage;
}

int NoiseBilateral::calcVal(int x, int y, Channel channel)
{
    math::matrix<float> window = getWindow(x, y, (radius*2)+1, channel, RepeatEdge);

    int size = window.rowno();
    float numeratorSum = 0, denominatorSum = 0;
    int v;
    int pixel = image->pixel(x, y);

    if (channel == RChannel) v = qRed(pixel);
    if (channel == GChannel) v = qGreen(pixel);
    if (channel == BChannel) v = qBlue(pixel);
    if (channel == LChannel) v = qGray(pixel);

    for (int x=0; x<size; x++)
         for (int y=0; y<size; y++)
         {
             float numerator = window(x, y) * colorCloseness(window(x, y), v) * spatialCloseness(QPoint(x - radius, y - radius), QPoint(x, y));
             float denominator = colorCloseness(window(x, y), v) * spatialCloseness(QPoint(x - radius, y - radius), QPoint(x, y));
             numeratorSum = numeratorSum + numerator;
             denominatorSum = denominatorSum + denominator;
         }

    return numeratorSum / denominatorSum;
}

float NoiseBilateral::colorCloseness(int val1, int val2)
{
    return pow(M_E, ((-1) * pow(val1 - val2, 2)) / (2 * pow(sigma_r, 2)));
}

float NoiseBilateral::spatialCloseness(QPoint point1, QPoint point2)
{
    return pow(M_E, ((-1) * (pow(point1.x() - point2.x(), 2) + pow(point1.y() - point2.y(), 2)) / (2 * pow(sigma_d, 2))));
}

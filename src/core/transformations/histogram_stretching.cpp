#include "histogram_stretching.h"

#include "../histogram.h"


HistogramStretching::HistogramStretching(PNM* img) :
    Transformation(img)
{
}

HistogramStretching::HistogramStretching(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HistogramStretching::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    if (image->format() == QImage::Format_Indexed8)
    {
        Histogram* histogram = image->getHistogram();
        histogram->get(Histogram::LChannel);
        QHash<int, int> channel = *histogram->get(Histogram::LChannel);

        // min and max
        int min = 0;
        int max = 255;

        QHash<int, int>::const_iterator it;
        for(it = channel.constBegin(); it!= channel.constEnd(); it++)
        {
            int key = it.key();
            if( key > 0 )
            {
                if (key < min) min = key;
                if (key > max) max = key;
            }
        }

        for (int x=0; x<width; x++)
        {
            for (int y=0; y<height; y++)
            {
                QRgb pixel = image->pixel(x,y);
                int gray = qGray(pixel);
                int new_gray = (int)floor(((double)(gray - min) / (double)(max - min)) * 255.0);
                newImage->setPixel(x, y, new_gray);
            }
        }

    }
    else
    {
        Histogram* histogram = image->getHistogram();
        histogram->get(Histogram::RChannel);
        QHash<int, int> RChannel = *histogram->get(Histogram::RChannel);
        QHash<int, int> GChannel = *histogram->get(Histogram::GChannel);
        QHash<int, int> BChannel = *histogram->get(Histogram::BChannel);

        QHash<int, int>::const_iterator it_r;
        QHash<int, int>::const_iterator it_g;
        QHash<int, int>::const_iterator it_b;
        int minR = 0, minG = 0, minB = 0;
        int maxR = 0, maxG = 0, maxB = 0;

        for(it_r = RChannel.constBegin(); it_r!= RChannel.constEnd(); it_r++)
        {
            int key = it_r.key();
            if( key > 0 )
            {
                 if (key < minR) minR = key;
                 if (key > maxR) maxR = key;
            }
        }
        for(it_g = GChannel.constBegin(); it_g!= GChannel.constEnd(); it_g++)
        {
            int key = it_g.key();
            if( key > 0 )
            {
                 if (key < minG) minG = key;
                 if (key > maxG) maxG = key;
            }
        }
        for(it_b = BChannel.constBegin(); it_b!= BChannel.constEnd(); it_b++)
        {
            int key = it_b.key();
            if( key > 0 )
            {
                 if (key < minB) minB = key;
                 if (key > maxB) maxB = key;
            }

        }

        for (int x=0; x<width; x++)
        {
            for (int y=0; y<height; y++)
            {
                 QRgb pixel = image->pixel(x, y);

                 int r = qRed(pixel);
                 int g = qGreen(pixel);
                 int b = qBlue(pixel);

                 int new_r = (int)((255.0 / (maxR - minR)) * (r - minR));
                 int new_g = (int)((255.0 / (maxG - minG)) * (g - minG));
                 int new_b = (int)((255.0 / (maxB - minB)) * (b - minB));

                 QColor newPixel = QColor(new_r,new_g,new_b);
                 newImage->setPixel(x,y,newPixel.rgb());

            }
        }

    }

    return newImage;
}


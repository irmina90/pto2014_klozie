#include "histogram_equalization.h"

#include "../histogram.h"

HistogramEqualization::HistogramEqualization(PNM* img) :
    Transformation(img)
{
}

HistogramEqualization::HistogramEqualization(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HistogramEqualization::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    if (image->format() == QImage::Format_Indexed8)
    {
        Histogram* histogram = image->getHistogram();
        histogram->get(Histogram::LChannel);
        QHash<int, int> channel = *histogram->get(Histogram::LChannel);

        QHash<int, int>::const_iterator it;
        double p[256]={0};
        double m = width * height;

        for (int i = 1; i < 256; i++)
        {
            p[i] = 0.0;
        }

        for (it = channel.constBegin(); it!= channel.constEnd(); it++)
        {
            p[it.key()] = it.value() / m;
        }

        double d[256] = {0};

        for (int i=1; i<256; i++)
        {
            if (i==0) d[i] = p[i];
            else d[i] = d[i-1] + p[i];

        }

        for (int x=0; x<width; x++)
        {
            for (int y=0; y<height; y++)
            {
                QRgb pixel = image->pixel(x,y);
                int gray = qGray(pixel);
                newImage->setPixel(x, y, (int)(d[gray]*255));
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

        double pR[256]={0};
        double pG[256]={0};
        double pB[256]={0};
        double dR[256]={0};
        double dG[256]={0};
        double dB[256]={0};

        for (int i = 0; i < 256; i++)
        {
            pR[i] = 0.0;
            pG[i] = 0.0;
            pB[i] = 0.0;
        }

        double m = width * height;

        for(it_r = RChannel.constBegin(); it_r!= RChannel.constEnd(); it_r++)
        {
            pR[it_r.key()] += it_r.value() / m;
        }
        for(it_g = GChannel.constBegin(); it_g!= GChannel.constEnd(); it_g++)
        {
            pG[it_g.key()] += it_g.value() / m;
        }
        for(it_b = BChannel.constBegin(); it_b!= BChannel.constEnd(); it_b++)
        {
            pB[it_b.key()] += it_b.value() / m;
        }

        for (int i=0; i<256; i++)
        {
            if ( i == 0 )
            {
                dR[i] = pR[i];
                dG[i] = pG[i];
                dB[i] = pB[i];
            }
            else
            {
                dR[i] = dR[i-1] + pR[i];
                dG[i] = dG[i-1] + pG[i];
                dB[i] = dB[i-1] + pB[i];
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

                 int new_r = int(dR[r]*255);
                 int new_g = int(dG[g]*255);
                 int new_b = int(dB[b]*255);

                 QColor newPixel = QColor(new_r,new_g,new_b);
                 newImage->setPixel(x,y,newPixel.rgb());

            }
        }

    }

    return newImage;
}


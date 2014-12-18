#include "hough_lines.h"

#include "bin_gradient.h"
#include "edge_laplacian.h"
#include "hough.h"

#include <QPainter>

HoughLines::HoughLines(PNM* img) :
    Transformation(img)
{
}

HoughLines::HoughLines(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HoughLines::transform()
{
    // Cut of value from the image;
    int  threshold      = getParameter("threshold").toInt();
    bool drawWholeLines = getParameter("draw_whole_lines").toBool();

    PNM* newImage = new PNM(image->copy());
    PNM* edgeImage, *tempImage;

    EdgeLaplacian lapl(image);
    lapl.setParameter("size", 3);
    edgeImage = lapl.transform();
    edgeImage = BinarizationGradient(edgeImage).transform();

    Hough hough(edgeImage);
    hough.setParameter("theta_density", 3);
    hough.setParameter("skip_edge_detection", true);
    tempImage = hough.transform();

    QPainter* painter = new QPainter(newImage);
    painter->setPen(Qt::red);

    for(int theta=0; theta < tempImage->width(); theta++)
    {
        for(int rho=0; rho < tempImage->height(); rho++)
        {
            if(qGray(tempImage->pixel(theta, rho)) > threshold)
            {
                double rtheta = (theta/3.0)*M_PI/180.0;
                int real_rho = rho - tempImage->height()/2;

                double sintheta = sin(rtheta);
                double costheta = cos(rtheta);

                painter->drawLine(0, (int)real_rho/sintheta, newImage->width()-1, (int)(real_rho - (newImage->width()-1)*costheta)/sintheta);
            }
        }
    }


    if(drawWholeLines == false)
    {
        for(int i=0; i<newImage->width(); i++)
        {
            for(int j=0; j<newImage->height(); j++)
            {
                if(qGray(edgeImage->pixel(i,j)) == 0){
                    newImage->setPixel(i, j, image->pixel(i,j));
                }
            }
        }
    }

    return newImage;
}

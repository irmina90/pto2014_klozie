#include "corner_harris.h"

#include "blur_gaussian.h"
#include "conversion_grayscale.h"
#include "edge_sobel.h"

CornerHarris::CornerHarris(PNM* img) :
    Convolution(img)
{
}

CornerHarris::CornerHarris(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* CornerHarris::transform()
{
    int    threshold    = getParameter("threshold").toInt();
    double sigma        = getParameter("sigma").toDouble(),
           sigma_weight = getParameter("sigma_weight").toDouble(),
           k_param      = getParameter("k").toDouble();

    int width  = image->width(),
        height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    math::matrix<float> Ixx(width, height);
    math::matrix<float> Iyy(width, height);
    math::matrix<float> Ixy(width, height);
    math::matrix<float> corner_candidates(width, height);
    math::matrix<float> corner_nonmax_suppress(width, height);

    PNM* tmpImage = ConversionGrayscale(image).transform();

    BlurGaussian blurGaussian(tmpImage);
    blurGaussian.setParameter("size", 3);
    blurGaussian.setParameter("sigma", 1.6);
    tmpImage = blurGaussian.transform();

    EdgeSobel edgeSobel(tmpImage);
    math::matrix<float>* Gx = edgeSobel.rawHorizontalDetection();
    math::matrix<float>* Gy = edgeSobel.rawVerticalDetection();

    for (int i=0; i<width; i++)
        for (int j=0; j<height; j++){
             Ixx(i, j) = (*Gx)(i, j) * (*Gx)(i, j);
             Iyy(i, j) = (*Gy)(i, j) * (*Gy)(i, j);
             Ixy(i, j) = (*Gx)(i, j) * (*Gy)(i, j);
        }

    for(int i=0; i<width; i++)
       for(int j=0; j<height; j++){

        if (i!=0 && j!=0)
        {
           float topSxx=0, topSyy=0, topSxy=0;

           for (int k=-1; k<=1; k++)
               for (int l=-1; l<=1; l++) {

                    topSxx += Ixx(i+k, j+l) * BlurGaussian::getGauss(k,l,sigma);
                    topSyy += Iyy(i+k, j+l) * BlurGaussian::getGauss(k,l,sigma);
                    topSxy += Ixy(i+k, j+l) * BlurGaussian::getGauss(k,l,sigma);
            }

           float Sxx = topSxx/sigma_weight;
           float Syy = topSyy/sigma_weight;
           float Sxy = topSxy/sigma_weight;

           math::matrix<float> H(2, 2);
           H(0, 0) = Sxx; H(0, 1) = Sxy;
           H(1, 0) = Sxy; H(1, 1) = Syy;
           float r = H.det() - k_param * pow(H(0, 0) + H(1, 1), 2);

           if (r > threshold) corner_candidates(i, j) = r;

        }
       }

    bool search = true;

    while(search)
    {
        search = false;
        for (int i=1; i<width; i++)
            for (int j=1; j<height; j++) {

              if (i!=0 && j!=0)
              {
                if (corner_candidates(i, j) > corner_candidates(i-1, j-1) && corner_candidates(i, j) > corner_candidates(i-1, j)
                && corner_candidates(i, j) > corner_candidates(i-1, j+1) && corner_candidates(i, j) > corner_candidates(i, j-1)
                && corner_candidates(i, j) > corner_candidates(i, j+1) && corner_candidates(i, j) > corner_candidates(i+1, j-1)
                && corner_candidates(i, j) > corner_candidates(i+1, j) && corner_candidates(i, j) > corner_candidates(i+1, j+1))
                {
                    corner_nonmax_suppress(i, j) = corner_candidates(i, j);
                }
                else
                {
                    if (corner_candidates(i, j) > 0) search = true;

                    corner_nonmax_suppress(i, j) = 0;
                }
              }
            }
        corner_candidates = corner_nonmax_suppress;
    }

    for (int i=1; i<width; i++)
        for (int j=1; j<height; j++) {

            if (corner_candidates(i, j) == 0) newImage->setPixel(i, j, 0);
            else newImage->setPixel(i, j, 1);
        }

    return newImage;
}

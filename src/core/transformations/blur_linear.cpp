#include "blur_linear.h"

BlurLinear::BlurLinear(PNM* img) :
    Convolution(img)
{
}

BlurLinear::BlurLinear(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* BlurLinear::transform()
{
    int maskSize = getParameter("size").toInt();
    QList<QVariant> tmpMask = getParameter("mask").toList();
    bool normalize = getParameter("normalize").toBool();

    math::matrix<float> mask(maskSize, maskSize);

    for(int i=0; i<maskSize; i++)
    {
       for(int j=0; j<maskSize; j++)
       {
         mask(i, j) = tmpMask.at(i).toDouble();
       }
    }

    if(normalize)
    {
      float sumMask = sum(mask);

      for(int i=0; i<maskSize; i++)
      {
          for(int j=0; j<maskSize; j++)
           {
                mask(i, j) = mask(i, j) / sumMask;
           }
      }
    }

    return convolute(mask, RepeatEdge);
}

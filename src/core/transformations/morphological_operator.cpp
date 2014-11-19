#include "morphological_operator.h"

MorphologicalOperator::MorphologicalOperator(PNM* img) :
    Transformation(img)
{
}

MorphologicalOperator::MorphologicalOperator(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

// abstract
const int MorphologicalOperator::morph(math::matrix<float>, math::matrix<bool>)
{
    return 0;
}

math::matrix<bool> MorphologicalOperator::getSE(int size, SE shape)
{
    switch (shape)
    {
    case Square:    return seSquare(size);
    case Cross:     return seCross(size);
    case XCross:    return seXCross(size);
    case VLine:     return seVLine(size);
    case HLine:     return seHLine(size);
    default:        return seSquare(size);
    }
}


math::matrix<bool> MorphologicalOperator::seSquare(int size)
{
    math::matrix<bool> ret(size, size);

    for (int i=0; i<size;i++)
    {
       for (int j=0;j<size;j++)
       {
           ret(i,j)=true;
       }
    }

    return ret;
}

math::matrix<bool> MorphologicalOperator::seCross(int size)
{
    math::matrix<bool> ret(size, size);

    for (int i=0; i<size;i++)
    {
      for (int j=0;j<size;j++)
      {
          if(i==(size/2) || j==(size/2))
          {
              ret(i,j)=true;
          }
          else
          {
              ret(i,j)=false;
          }
      }
    }

    return ret;
}

math::matrix<bool> MorphologicalOperator::seXCross(int size)
{
    math::matrix<bool> ret(size, size);

    for (int i=0; i<size;i++)
    {
        for (int j=0;j<size;j++)
        {
            if(i==j || i==(size-j))
            {
                ret(i,j)=true;
            }
            else
            {
                ret(i,j)=false;
            }
        }
    }

    return ret;
}

math::matrix<bool> MorphologicalOperator::seVLine(int size)
{
    math::matrix<bool> ret(size, size);

    for (int i=0; i<size;i++)
    {
       for (int j=0;j<size;j++)
       {
           if(i==(size/2))
           {
               ret(i,j)=true;
           }
           else
           {
               ret(i,j)=false;
           }
       }
    }

    return ret;
}

math::matrix<bool> MorphologicalOperator::seHLine(int size)
{
    math::matrix<bool> ret(size, size);

    for (int i=0;i<size;i++)
    {
         for (int j=0;j<size;j++)
         {
             if(j==(size/2))
             {
                 ret(i,j)=true;
             }
             else
             {
                 ret(i,j)=false;
             }
          }
     }

    return ret;
}

PNM* MorphologicalOperator::transform()
{  
    int size  = getParameter("size").toInt();
    SE  shape = (MorphologicalOperator::SE) getParameter("shape").toInt();

    PNM* newImage = new PNM(image->width(), image->height(), QImage::Format_RGB32);

    math::matrix<bool> newShape = getSE(size,shape);

     if(image->format() == QImage::Format_Indexed8)
     {

         for(int i=0;i<image->width();i++)
         {
             for(int j=0;j<image->height();j++)
             {
                 double g = morph(getWindow(i, j, size, LChannel, RepeatEdge), newShape);

                 newImage->setPixel(i, j, qRgb(g, g, g));
             }
         }

     }
     else
     {
         for(int i=0;i<image->width();i++)
         {
             for(int j=0;j<image->height();j++)
             {
                 double r = morph(getWindow(i, j, size, RChannel, RepeatEdge), newShape);
                 double g = morph(getWindow(i, j, size, GChannel, RepeatEdge), newShape);
                 double b = morph(getWindow(i, j, size, BChannel, RepeatEdge), newShape);

                 newImage->setPixel(i, j, qRgb(r,g,b));
             }
         }
     }


    return newImage;
}

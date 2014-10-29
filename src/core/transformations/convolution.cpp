#include "convolution.h"

/** Overloaded constructor */
Convolution::Convolution(PNM* img) :
    Transformation(img)
{
}

Convolution::Convolution(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

/** Returns a convoluted form of the image */
PNM* Convolution::transform()
{
    return convolute(getMask(3, Normalize), RepeatEdge);
}

/** Returns a sizeXsize matrix with the center point equal 1.0 */
math::matrix<float> Convolution::getMask(int size, Mode mode = Normalize)
{
    math::matrix<float> mask(size, size);

    for (int i = 0; i < size; i++)
       for (int j = 0; j < size; j++)
       {
          mask(i, j) = 0;
       }

        mask(size/2, size/2) = 1;

    return mask;
}

/** Does the convolution process for all pixels using the given mask. */
PNM* Convolution::convolute(math::matrix<float> mask, Mode mode = RepeatEdge)
{
    int width  = image->width(),
        height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    float weight = sum(mask);

    if (image->format() == QImage::Format_Indexed8)
        {

            for (int i=0; i<width; i++)
                for (int j=0; j<height; j++)
                {
                    math::matrix<float> window = getWindow(i, j, mask.rowno(), LChannel, mode);
                    math::matrix<float> accumulator = join(window, reflection(mask));

                    float sumAccumulator = sum(accumulator);
                    if (weight !=0 ) sumAccumulator = sumAccumulator / weight;

                    if (sumAccumulator < 0) sumAccumulator = 0;
                    if (sumAccumulator > 255) sumAccumulator = 255;

                    newImage->setPixel(i, j, sumAccumulator);
                }

        }
        else if (image->format() == QImage::Format_RGB32)
        {

            for (int i=0; i<width; i++)
                for (int j=0; j<height; j++)
                {
                    //RChannel
                    math::matrix<float> window = getWindow(i, j, mask.rowno(), RChannel, mode);
                    math::matrix<float> accumulator = join(window, reflection(mask));

                    float sumAccumulatorR = sum(accumulator);
                    if (weight !=0 ) sumAccumulatorR = sumAccumulatorR / weight;

                    if (sumAccumulatorR < 0) sumAccumulatorR = 0;
                    if (sumAccumulatorR > 255) sumAccumulatorR = 255;

                    //GChannel
                    window = getWindow(i, j, mask.rowno(), GChannel, mode);
                    accumulator = join(window, reflection(mask));

                    float sumAccumulatorG = sum(accumulator);
                    if (weight !=0 ) sumAccumulatorG = sumAccumulatorG / weight;

                    if (sumAccumulatorG < 0) sumAccumulatorG = 0;
                    if (sumAccumulatorG > 255) sumAccumulatorG = 255;

                    //BChanne
                    window = getWindow(i, j, mask.rowno(), BChannel, mode);
                    accumulator = join(window, reflection(mask));

                    float sumAccumulatorB = sum(accumulator);
                    if (weight !=0 ) sumAccumulatorB = sumAccumulatorB / weight;

                    if (sumAccumulatorB < 0) sumAccumulatorB = 0;
                    if (sumAccumulatorB > 255) sumAccumulatorB = 255;

                    QRgb pixel = image->pixel(i, j); // Getting the pixel(x,y) value
                    int r = qRed(pixel);
                    int g = qGreen(pixel);
                    int b = qBlue(pixel);
                    r = sumAccumulatorR;
                    g = sumAccumulatorG;
                    b = sumAccumulatorB;

                    QColor newPixel = QColor(r,g,b);
                    newImage->setPixel(i, j, newPixel.rgb());

                }
        }

    return newImage;
}

/** Joins to matrices by multiplying the A[i,j] with B[i,j].
  * Warning! Both Matrices must be squares with the same size!
  */
const math::matrix<float> Convolution::join(math::matrix<float> A, math::matrix<float> B)
{
    int size = A.rowno();
    math::matrix<float> C(size, size);

    for (int i = 0; i < size; i++)
         for (int j = 0; j < size; j++)
         {
             C(i, j) = A(i, j) * B(i, j);
         }

    return C;
}

/** Sums all of the matrixes elements */
const float Convolution::sum(const math::matrix<float> A)
{
    float sum = 0.0;
    int size = A.rowno();

    for (int i = 0; i < size; i++)
         for (int j = 0; j < size; j++)
         {
             sum = sum + A(i, j);
         }

    return sum;
}


/** Returns reflected version of a matrix */
const math::matrix<float> Convolution::reflection(const math::matrix<float> A)
{
    int size = A.rowno();
    math::matrix<float> C(size, size);

    for (int i = 0; i < size; i++)
         for (int j = 0; j < size; j++)
         {
            int x = size - i - 1;
            int y = size - j - 1;
            C(i, j) = A(x, y);
         }

    return C;
}

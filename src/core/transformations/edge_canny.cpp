#include "edge_canny.h"

#include "blur_gaussian.h"
#include "conversion_grayscale.h"
#include "edge_sobel.h"

EdgeCanny::EdgeCanny(PNM* img) :
    Convolution(img)
{
}

EdgeCanny::EdgeCanny(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

int EdgeCanny::orientation(float o)
{

    if ((o >= 112.5 && o < 157.5) || (o >= 292.5 && o < 337.5)) {
                return 3;
    }
    else if ((o >= 67.5 && o < 112.5) || (o >= 247.5 && o < 292.5)) {
                return 2;
    }
    else if ((o >= 22.5 && o < 67.5) || (o >= 202.5 && o < 247.5)) {
                return 1;
    }
    else if ((o < 22.5) || (o >= 157.5 && o < 202.5) || (o >= 337.5)) {
                return 0;
    }
}

PNM* EdgeCanny::transform()
{
    int width  = image->width(),
        height = image->height();

    int upper_thresh = getParameter("upper_threshold").toInt(),
        lower_thresh = getParameter("lower_threshold").toInt();

    PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);

    newImage = ConversionGrayscale(image).transform();

        BlurGaussian blurGaussian(newImage);
        blurGaussian.setParameter("size", 3);
        blurGaussian.setParameter("sigma", 1.6);
        newImage = blurGaussian.transform();

        EdgeSobel edgeSobel(newImage);
        math::matrix<float>* Gx = edgeSobel.rawHorizontalDetection();
        math::matrix<float>* Gy = edgeSobel.rawVerticalDetection();

        math::matrix<float> m = math::matrix<float>(width, height);
        math::matrix<float> o = math::matrix<float>(width, height);

        for (int i=0; i<width; i++)
            for (int j=0; j<height; j++) {

                m(i, j) = sqrt(pow((*Gx)(i, j), 2) + pow((*Gy)(i, j), 2));
                float currentDirection = (atan2((*Gy)(i, j), (*Gx)(i, j)) / M_PI);

                if (currentDirection < 0) currentDirection = (-1) * currentDirection + 180;
                o(i, j) = currentDirection;
            }

             std::list <QPoint> list;

             for (int i=1; i<width-1; i++)
                 for (int j = 1; j < height - 1; j++) {

                     int direction = orientation(o(i, j));
                     int neighbor1, neighbor2;

                     if (direction == 0) neighbor1 = m(i, j - 1); neighbor2 = m(i, j + 1);

                     if (direction == 1) neighbor1 = m(i + 1, j - 1); neighbor2 = m(i - 1, j + 1);

                     if (direction == 2) neighbor1 = m(i - 1, j); neighbor2 = m(i + 1, j);

                     if (direction == 3) neighbor1 = m(i - 1, j - 1); neighbor2 = m(i + 1, j + 1);


                     if (m(i, j) > neighbor1 && m(i, j) > neighbor2 && m(i, j) > upper_thresh)
                     {
                         newImage->setPixel(i, j, PIXEL_VAL_MIN);
                         QPoint point(i, j);
                         list.push_back(point);
                      }
                      else
                      {
                         newImage->setPixel(i, j, 0);
                      }
                  }

                  while (!(list.empty()))
                  {
                      QPoint current = list.back();
                      list.pop_back();

                      int x = current.x();
                      int y = current.y();

                      int direction = orientation(o(x, y));

                      int neighbor_x[7] = {-1, -1, 0, 0, 1, 1, 1};
                      int neighbor_y[7] = {0, 1, -1, 1, -1, 0, 1};


                      for (int i=0; i <= 1; i++)
                      {
                          int cX;
                          int cY;

                          if (i == 0)
                          {
                              if (direction == 0) cX = 0; cY = -1;
                              if (direction == 1) cX = 1; cY = -1;
                              if (direction == 2) cX = -1; cY = 0;
                              if (direction == 3) cX = -1; cY = -1;
                          }
                          else
                          {
                              if (direction == 0) cX = 0; cY = 1;
                              if (direction == 1) cX = -1; cY = 1;
                              if (direction == 2) cX = 1; cY = 0;
                              if (direction == 3) cX = 1; cY = 1;
                          }

                          while (true)
                          {
                              int nextX = x + cX;
                              int nextY = y + cY;

                              if (nextX == width-1 || nextX == 0 || nextY == height-1 || nextY == 0) break;
                              if (newImage->pixel(nextX, nextY) == PIXEL_VAL_MIN) break;
                              if (m(nextX, nextY) < lower_thresh) break;
                              if (orientation(o(nextX, nextY)) != direction) break;

                              bool max = true;
                              for (int i = 0; i < 7; i++)
                               {
                                  int tmpX = nextX + neighbor_x[i];
                                  int tmpY = nextY + neighbor_y[i];

                                  if ((tmpX == x && tmpY == y)) continue;
                                  if (orientation(o(tmpX, tmpY)) != direction) continue;

                                  if (m(tmpX, tmpY) >= m(nextX, nextY))
                                  {
                                     max = false;
                                     break;
                                  }
                                }

                                if (!max) break;

                                newImage->setPixel(nextX, nextY, PIXEL_VAL_MIN);
                                x = nextX;
                                y = nextY;
                            }
                        }
                 }

        return newImage;
}

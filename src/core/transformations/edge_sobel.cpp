#include "edge_sobel.h"

EdgeSobel::EdgeSobel(PNM* img, ImageViewer* iv) :
    EdgeGradient(img, iv)
{
    prepareMatrices();
}

EdgeSobel::EdgeSobel(PNM* img) :
    EdgeGradient(img)
{
    prepareMatrices();
}

void EdgeSobel::prepareMatrices()
{
    g_x = math::matrix<double>(3,3);
    g_y = math::matrix<double>(3,3);

    g_x(0,0) = -1;
    g_x(0,1) = 0;
    g_x(0,2) = 1;
    g_x(1,0) = -2;
    g_x(1,1) = 0;
    g_x(1,2) = 2;
    g_x(2,0) = -1;
    g_x(2,1) = 0;
    g_x(2,2) = 1;

    g_y(0,0) = -1;
    g_y(0,1) = -2;
    g_y(0,2) = -1;
    g_y(1,0) = 0;
    g_y(1,1) = 0;
    g_y(1,2) = 0;
    g_y(2,0) = 1;
    g_y(2,1) = 2;
    g_y(2,2) = 1;

}

math::matrix<float>* EdgeSobel::rawHorizontalDetection()
{
    math::matrix<float>* x_gradient = new math::matrix<float>(this->image->width(), this->image->height());

    for (int unsigned i=0; i<x_gradient->rowno(); i++)
    {
        for (int unsigned j=0; j<x_gradient->colno(); j++){

            math::matrix<float> window = getWindow(i,j,3,LChannel,RepeatEdge);
            float suma = sum(join(g_x,window));
            (*x_gradient)(i,j) = suma;

        }
    }

    return x_gradient;
}

math::matrix<float>* EdgeSobel::rawVerticalDetection()
{
    math::matrix<float>* y_gradient = new  math::matrix<float>(this->image->width(), this->image->height());

    for (int unsigned i=0; i<y_gradient->rowno(); i++)
    {
        for (int unsigned j=0; j<y_gradient->colno(); j++){

            math::matrix<float> window = getWindow(i,j,3,LChannel,RepeatEdge);
            float suma = sum(join(g_y,window));
            (*y_gradient)(i,j) = suma;

        }
    }

    return y_gradient;
}

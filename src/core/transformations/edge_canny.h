#ifndef EDGECANNY_H
#define EDGECANNY_H

#include "convolution.h"

class EdgeCanny : public Convolution
{
public:
    EdgeCanny(PNM*);
    EdgeCanny(PNM*, ImageViewer*);
    int orientation(float);
    virtual PNM* transform();


private:

};

#endif // EDGECANNY_H

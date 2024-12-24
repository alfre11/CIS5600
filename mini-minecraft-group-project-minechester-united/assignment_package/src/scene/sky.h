#ifndef SKY_H
#define SKY_H

#include "drawable.h"

class Sky : public Drawable {
public:
    Sky(OpenGLContext*);
    ~Sky();

    // A function that initializes the vertex
    // coordinates for a Skyrangle that covers
    // the entire screen in screen space,
    // then buffers that data to the GPU.
    void createVBOdata() override;
};

#endif // SKY_H

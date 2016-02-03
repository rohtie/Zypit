#pragma once

#include "constants.h"

class Clip {
    public:
        string src = "shader.frag";

        float time = 0.0;

        int start = 0;
        int length = 25;

        Clip *left  = NULL;
        Clip *right = NULL;

    void draw() {
        ofSetColor(128);
        ofDrawRectangle(start, 0, length, TIMELINE_HEIGHT);
    }
};

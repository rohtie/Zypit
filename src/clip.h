#pragma once

#include "ofMain.h"
#include "constants.h"


class Clip {
    public:
        string src = "shader.frag";

        float time = 0.0;

        int start = 0;
        int length = 25;

        Clip *left  = NULL;
        Clip *right = NULL;

        ofRectangle rect;

    Clip() {
        rect.x = start;
        rect.width = length;
        rect.height = TIMELINE_HEIGHT;
    }

    void draw() {
        ofSetColor(128);
        ofDrawRectangle(rect);
    }
};

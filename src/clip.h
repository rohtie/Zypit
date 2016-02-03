#pragma once

#include "ofMain.h"
#include "constants.h"


class Clip {
    public:
        string src = "shader.frag";

        float time = 0.0;

        int start;
        int length;

        Clip *left  = NULL;
        Clip *right = NULL;

        ofRectangle rect;

        ofTrueTypeFont font;

    Clip(int _start, int _length, string _src, ofTrueTypeFont &_font) {
        start = _start;
        length = _length;
        src = _src;

        rect.x = start;
        rect.width = length;
        rect.height = TIMELINE_HEIGHT;
        font = _font;
    }

    void draw() {
        ofSetColor(128);
        ofDrawRectangle(rect);
        ofSetColor(255);
        font.drawString(src, rect.x, rect.height / 2 + TIMELINE_FONT_SIZE / 2);
    }
};

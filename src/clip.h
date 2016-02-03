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

        bool isSelected = false;
        int selectedPos = 0;

    Clip(int _start, int _length, string _src, ofTrueTypeFont &_font) {
        start = _start;
        length = _length;
        src = _src;
        font = _font;

        rect.height = TIMELINE_HEIGHT;

        reconstruct();
    }

    void reconstruct() {
        rect.x = start;
        rect.width = length;
    }

    void inside(int x, int y) {
        if (rect.inside(x, y)) {
            isSelected = true;
            selectedPos = x - rect.x;
        }
    }

    void deSelect() {
        isSelected = false;
    }

    void update(int x, int y) {
        if (isSelected) {
            start = x - selectedPos;
            reconstruct();
        }
    }

    void draw() {
        ofSetColor(128);
        ofDrawRectangle(rect);
        ofSetColor(255);
        font.drawString(src, rect.x, rect.height / 2 + TIMELINE_FONT_SIZE / 2);
    }
};

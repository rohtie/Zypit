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

    void swap(Clip *a, Clip *b) {
        Clip *aLeft = b;
        Clip *aRight = b->right;

        Clip *bLeft = a->left;
        Clip *bRight = a;

        if (a->left == b) {
            aLeft = b->left;
            aRight = b;

            bLeft = a;
            bRight = a->right;

            if (b->left != NULL) {
                b->left->right = a;
            }

            if (a->right != NULL) {
                a->right->left = b;
            }
        }
        else if (a->right = b) {
            if (b->right != NULL) {
                b->right->left = a;
            }

            if (a->left != NULL) {
                a->left->right = b;
            }
        }

        a->left = aLeft;
        a->right = aRight;

        b->left = bLeft;
        b->right = bRight;
    }

    int update(int x, int y) {
        if (isSelected) {
            start = x - selectedPos;

            if (left != NULL && start < left->start + left->length) {
                if (start <= left->start) {
                    start = left->start;
                    left->start = start + length;

                    reconstruct();
                    left->reconstruct();

                    swap(this, left);
                    return 1;
                }
                else {
                    start = left->start + left->length;
                }
            }
            else if (right != NULL && start + length > right->start) {
                if (start + length >= right->start + right->length) {
                    start = right->start + right->length - length;
                    right->start = right->start - length;

                    reconstruct();
                    right->reconstruct();

                    swap(this, right);
                    return 1;
                }
                else {
                    start = right->start - length;
                }
            }
            else if (start < 0) {
                start = 0;
            }

            reconstruct();
        }
        return 0;
    }

    void draw() {
        ofSetColor(128);
        ofDrawRectangle(rect);
        ofSetColor(255);
        font.drawString(src, rect.x, rect.height / 2 + TIMELINE_FONT_SIZE / 2);
    }
};

#pragma once

#include "ofMain.h"
#include "constants.h"

#include "shader.h"


class Clip : public Shader {
    public:
        float time = 0.0;

        int start;
        int length;

        Clip *left  = NULL;
        Clip *right = NULL;

        ofRectangle rect;

        bool isSelected = false;
        int selectedPos = 0;

        bool isExpanding = false;
        bool isLeftExpand = false;
        int orgStart = 0;
        int orgLength = 0;

		#ifndef STANDALONE_PLAYER
		ofTrueTypeFont font;
		#endif

	#ifndef STANDALONE_PLAYER
    Clip(string _src, int _start, int _length, float _time, string* _iChannelSrc, string* _iChannelFilter, ofTrueTypeFont &_font) : Shader(_src, _iChannelSrc, _iChannelFilter) {
	#else
	Clip(string _src, int _start, int _length, float _time, string* _iChannelSrc, string* _iChannelFilter) : Shader(_src, _iChannelSrc, _iChannelFilter) {
	#endif
        start = _start;
        length = _length;
        time = _time;
		rect.height = TIMELINE_CLIP_HEIGHT;
        rect.y = TIMELINE_HEIGHT - TIMELINE_CLIP_HEIGHT;

		#ifndef STANDALONE_PLAYER
		font = _font;
        reconstruct();
		#endif
    }

	#ifndef STANDALONE_PLAYER
    void reconstruct() {
        rect.x = start;
        rect.width = length;
    }

    void inside(int x, int y) {
        if (rect.inside(x, y)) {
            if (x < start + TIMELINE_EXPAND_AREA) {
                isExpanding = true;
                isLeftExpand = true;
            }
            else if (x > start + length - TIMELINE_EXPAND_AREA) {
                isExpanding = true;
                isLeftExpand = false;
            }
            else {
                isSelected = true;
                selectedPos = x - rect.x;
            }

            orgStart = start;
            orgLength = length;
        }
    }

    void deSelect() {
        isSelected = false;
        isExpanding = false;
        isLeftExpand = false;
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
        else if (a->right == b) {
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
        Shader::update();

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
        else if (isExpanding) {
            // TODO: Do not allow negative expand
            if (isLeftExpand) {
                start = x;
                length = orgLength + orgStart - x;
            }
            else {
                start = orgStart;
                length = x - orgStart;
            }

            if (left != NULL && start < left->start + left->length) {
                start = left->start + left->length;
                length = (orgStart + orgLength) - (left->start + left->length);
            }
            else if (right != NULL && start + length > right->start) {
                length = right->start - orgStart;
                start = right->start - length;
            }

            reconstruct();
        }

        return 0;
    }

    void draw() {
        ofSetColor(TIMELINE_CLIP_COLOR);
        ofDrawRectangle(rect);

        ofSetColor(TIMELINE_CLIP_COLOR + 28);
        ofDrawLine(rect.x + 1, rect.y, rect.x + 1, rect.y + TIMELINE_CLIP_HEIGHT);
        ofSetColor(TIMELINE_CLIP_COLOR - 28);
        ofDrawLine(rect.x + rect.width, rect.y, rect.x + rect.width, rect.y + TIMELINE_CLIP_HEIGHT);

        ofSetColor(TIMELINE_FONT_COLOR);
        font.drawString(src, rect.x + TIMELINE_FONT_SIZE / 2, rect.height / 2 + TIMELINE_FONT_SIZE / 2);
    }
	#endif
};

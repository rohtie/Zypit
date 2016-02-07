#include "ofApp.h"
#include "constants.h"
#include "clip.h"

void ofApp::setup() {
    palanquinRegular.loadFont("Palanquin-Regular.ttf", TIMELINE_FONT_SIZE);

    /* Normalize texture coordinates so that they are within 0 to 1 range */
    ofDisableArbTex();

    /* Make sure textures can be repeated */
    ofSetTextureWrap(GL_REPEAT, GL_REPEAT);

    ofBackground(BG_COLOR);

    /* Setup clips */
    defaultClip = new Clip(0, 200, "default", palanquinRegular);

    first = new Clip(0, 200, "sun", palanquinRegular);
    Clip *middle = new Clip(200, 10, "pyramidtrip", palanquinRegular);
    last = new Clip(210, 100, "drugbots", palanquinRegular);

    first->right = middle;
    middle->left = first;
    middle->right = last;
    last->left = middle;

    playing = first;
}

void ofApp::update() {
    /* Scroll timeline when within timeline area. */
    if (mouseY > ofGetHeight() - TIMELINE_HEIGHT) {
        if (mouseX < TIMELINE_SCOLLING_AREA) {
            timelinePos -= TIMELINE_SCOLLING_SPEED;
        }
        else if (mouseX > ofGetWidth() - TIMELINE_SCOLLING_AREA) {
            timelinePos += TIMELINE_SCOLLING_SPEED;
        }
    }

    int x = mouseX + timelinePos;
    int y = mouseY - ofGetHeight() - TIMELINE_HEIGHT;

    /* Update clips */
    Clip *current = first;
    while (current != NULL) {
        int res = current->update(x, y);

        /* After swapping, the current clip might have become the first or last clip in the list */
        if (res == 1) {
            if (current->left == NULL) {
                first = current;
            }
            else if (current->left->left == NULL) {
                first = current->left;
            }

            if (current->right == NULL) {
                last = current;
            }
            else if (current->right->right == NULL) {
                last = current->right;
            }
        }

        current = current->right;
    }

    /* Get current playing clip */
    if (isPlaying) {
        timelineMarker += 1;
        timelineMarker = timelineMarker % (last->start + last->length);

        playing = defaultClip;

        current = first;
        while (current != NULL) {
            if (timelineMarker >= current->start && timelineMarker <= current->start + current->length) {
                playing = current;
            }
            current = current->right;
        }
    }
}

void ofApp::draw() {
    int width = ofGetWidth();
    int height = ofGetHeight() - TIMELINE_HEIGHT;

    /* MAIN SCREEN */
    ofSetColor(255);
    playing->shader.begin();
        playing->shader.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        playing->shader.setUniform2f("iResolution", width, height);
        ofRect(0, 0, width, height);
    playing->shader.end();

    /* TIMELINE */
    timeline.allocate(last->start + last->length, TIMELINE_HEIGHT);
    timeline.begin();
        ofBackground(TIMELINE_BG_COLOR);

        Clip *current = first;
        while (current != NULL) {
            current->draw();
            current = current->right;
        }

        ofSetColor(255, 0, 0);
        ofDrawLine(timelineMarker, 0, timelineMarker, TIMELINE_HEIGHT);
        ofDrawRectangle(timelineMarker - TIMELINE_MARKER_SIZE / 2, 0, TIMELINE_MARKER_SIZE, TIMELINE_HEIGHT - TIMELINE_CLIP_HEIGHT);

    timeline.end();
    timeline.draw(-timelinePos, height);
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        isPlaying = !isPlaying;
    }
}

void ofApp::keyReleased(int key) {
}

void ofApp::mouseMoved(int x, int y) {
}

void ofApp::mouseDragged(int x, int y, int button) {
}

void ofApp::mousePressed(int x, int y, int button) {
    /* Make sure we click on the correct part of the timeline */
    x += timelinePos;
    y -= ofGetHeight() - TIMELINE_HEIGHT;

    Clip *current = first;
    while (current != NULL) {
        current->inside(x, y);
        current = current->right;
    }
}

void ofApp::mouseReleased(int x, int y, int button) {
    Clip *current = first;
    while (current != NULL) {
        current->deSelect();
        current = current->right;
    }
}

void ofApp::windowResized(int w, int h) {
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
}
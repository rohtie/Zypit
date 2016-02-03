#include "ofApp.h"
#include "constants.h"
#include "clip.h"

void ofApp::setup() {
    palanquinRegular.loadFont("Palanquin-Regular.ttf", TIMELINE_FONT_SIZE);

    /* Normalize texture coordinates so that they are within 0 to 1 range */
    ofDisableArbTex();

    /* Make sure textures can be repeated */
    ofSetTextureWrap(GL_REPEAT, GL_REPEAT);

    shader.load("shadersGL3/shader");

    /* Setup clips */
    first = new Clip(0, 100, "sun", palanquinRegular);
    last = new Clip(150, 200, "narves1", palanquinRegular);

    first->right = last;
    last->left = first;
}

void ofApp::update() {
    /* Scroll timeline when within timeline area. */
    if (mouseY > ofGetHeight() - TIMELINE_HEIGHT) {
        if (mouseX < TIMELINE_SCOLLING_AREA) {
            timelinePos -= 1;
        }
        else if (mouseX > ofGetWidth() - TIMELINE_SCOLLING_AREA) {
            timelinePos += 1;
        }
    }

    int x = mouseX + timelinePos;
    int y = mouseY - ofGetHeight() - TIMELINE_HEIGHT;

    Clip *current = first;
    while(current != NULL) {
        current->update(x, y);
        current = current->right;
    }
}

void ofApp::draw() {
    int width = ofGetWidth();
    int height = ofGetHeight() - TIMELINE_HEIGHT;

    /* MAIN SCREEN */
    ofSetColor(255);
    shader.begin();
    ofRect(0, 0, width, height);
    shader.end();

    /* TIMELINE */
    /* TODO: calculate width based on last frame of last clip */
    timeline.allocate(last->start + last->length, TIMELINE_HEIGHT);
    timeline.begin();

        Clip *current = first;
        while(current != NULL) {
            current->draw();
            current = current->right;
        }

    timeline.end();
    timeline.draw(-timelinePos, height);
}

void ofApp::keyPressed(int key) {
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
    while(current != NULL) {
        current->inside(x, y);
        current = current->right;
    }
}

void ofApp::mouseReleased(int x, int y, int button) {
    Clip *current = first;
    while(current != NULL) {
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
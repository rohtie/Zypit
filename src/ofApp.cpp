#include "ofApp.h"
#include "constants.h"
#include "clip.h"

void ofApp::setup() {
    palanquinRegular.loadFont("Palanquin-Regular.ttf", 15);

    // Normalize texture coordinates so that they are within 0 to 1 range
    ofDisableArbTex();

    // Make sure textures can be repeated
    ofSetTextureWrap(GL_REPEAT, GL_REPEAT);

    shader.load("shadersGL3/shader");
}

void ofApp::update() {
    if (mouseX < TIMELINE_SCOLLING_AREA) {
        timelinePos -= 1;
    }
    else if (mouseX > ofGetWidth() - TIMELINE_SCOLLING_AREA) {
        timelinePos += 1;
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

    ofSetColor(0);
    palanquinRegular.drawString("Hello!", width / 2, height / 2);

    /* TIMELINE */
    /* TODO: calculate width based on last frame of last clip */
    timeline.allocate(1920, TIMELINE_HEIGHT);
    timeline.begin();
        Clip test;
        test.draw();
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
}

void ofApp::mouseReleased(int x, int y, int button) {
}

void ofApp::windowResized(int w, int h) {
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
}
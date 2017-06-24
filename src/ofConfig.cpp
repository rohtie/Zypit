#include "pugi/pugixml.hpp"

#include "ofConfig.h"

const int RESOLUTION_FONT_SIZE = 15;
const int RESOLUTION_HEIGHT = 25;
const int RESOLUTION_MARGIN = 10;

ofConfig::ofConfig(ofGLWindowSettings* _settings) {
    settings = _settings;
}

void ofConfig::setup() {
    palanquinRegular.load("Palanquin-Regular.ttf", RESOLUTION_FONT_SIZE);

    // Load resolutions
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("resolutions.xml");

    int i = 0;

    for (pugi::xml_node resolution = doc.first_child(); resolution; resolution = resolution.next_sibling()) {

        if (resolution.attribute("default").as_bool()) {
            selectedResolution = i;
        }

        resolutions.push_back(make_pair(
            resolution.attribute("width").as_int(),
            resolution.attribute("height").as_int()));

        resolutionRects.push_back(new ofRectangle(
            RESOLUTION_MARGIN,
            i * (RESOLUTION_HEIGHT + RESOLUTION_MARGIN / 2) + RESOLUTION_MARGIN,
            ofGetWidth() - RESOLUTION_MARGIN * 2,
            RESOLUTION_HEIGHT));

        i++;
    }

    fullscreenRect.x = RESOLUTION_MARGIN;
    fullscreenRect.y = i * (RESOLUTION_HEIGHT + RESOLUTION_MARGIN);
    fullscreenRect.width = ofGetWidth() - RESOLUTION_MARGIN * 2;
    fullscreenRect.height = RESOLUTION_HEIGHT;

    i++;

    runDemoRect.x = ofGetWidth() * 0.5 + RESOLUTION_MARGIN;
    runDemoRect.y = i * (RESOLUTION_HEIGHT + RESOLUTION_MARGIN);
    runDemoRect.width = ofGetWidth() * 0.5 - RESOLUTION_MARGIN * 2;
    runDemoRect.height = RESOLUTION_HEIGHT;
}

void ofConfig::draw() {
    ofBackground(25);

    // Resolution choices
    for (int i = 0; i < resolutions.size(); i++) {
        if (i == selectedResolution) {
            ofSetColor(75);
        }
        else {
            ofSetColor(50);
        }

        ofDrawRectangle(*resolutionRects[i]);

        stringstream s;
        s << resolutions[i].first << " x " << resolutions[i].second;

        ofSetColor(150);
        palanquinRegular.drawString(
            s.str(),
            ofGetWidth() * 0.5 - (RESOLUTION_FONT_SIZE * 0.75 * s.str().length()) * 0.5,
            resolutionRects[i]->y + RESOLUTION_FONT_SIZE + RESOLUTION_MARGIN / 2);
    }

    // Fullscreen button
    if (isFullscreen) {
        ofSetColor(75);
    }
    else {
        ofSetColor(50);
    }
    ofDrawRectangle(fullscreenRect);

    ofSetColor(150);
    string s = "FULLSCREEN";
    palanquinRegular.drawString(
        s,
        ofGetWidth() * 0.5 - (RESOLUTION_FONT_SIZE * 0.75 * s.length()) * 0.5,
        fullscreenRect.y + RESOLUTION_FONT_SIZE + RESOLUTION_MARGIN / 2);

	/*
	TODO: Figure out how to close the window without segfaulting

    ofSetColor(50);
    ofDrawRectangle(runDemoRect);

    ofSetColor(150);
    s = "RUN DEMO";
    palanquinRegular.drawString(
        s,
        runDemoRect.x + runDemoRect.width * 0.5 - (RESOLUTION_FONT_SIZE * 0.75 * s.length()) * 0.5,
        runDemoRect.y + RESOLUTION_FONT_SIZE + RESOLUTION_MARGIN / 2);
    */

}

void ofConfig::mousePressed(int x, int y, int button) {
    // Select resolution if mouse is within rectangle boundaries
    for (int i = 0; i < resolutionRects.size(); i++) {
        if (resolutionRects[i]->inside(x, y)) {
            selectedResolution = i;

			settings->width = resolutions[selectedResolution].first;
			settings->height = resolutions[selectedResolution].second;
        }
    }

    // Toggle fullscreen button if mouse is within rectangle
    if (fullscreenRect.inside(x, y)) {
        isFullscreen = !isFullscreen;

        if (isFullscreen) {
            settings->windowMode = OF_FULLSCREEN;
        }
        else {
            settings->windowMode = OF_WINDOW;
        }
    }

    if (runDemoRect.inside(x, y)) {
		// TODO: figure out how to exit this window without
		//       fucking up the openframeworks context
    }
}

void ofConfig::windowResized(int w, int h) {
    // Make sure all rects are scaled alongside the window
    for (int i = 0; i < resolutionRects.size(); i++) {
        resolutionRects[i]->width = w - RESOLUTION_MARGIN * 2;
    }

    fullscreenRect.width = w - RESOLUTION_MARGIN * 2;
    runDemoRect.width = w * 0.5 - RESOLUTION_MARGIN * 2;
}

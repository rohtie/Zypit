#pragma once

#include "ofMain.h"

#include "clip.h"

class ofConfig : public ofBaseApp {
    public:
        ofTrueTypeFont palanquinRegular;

        vector<pair<int, int>> resolutions;
        vector<ofRectangle*> resolutionRects;

        ofRectangle fullscreenRect;
        bool isFullscreen = true;

        ofRectangle runDemoRect;

        int selectedResolution;

        ofGLWindowSettings* settings;
        ofConfig(ofGLWindowSettings* _settings);

        void setup();
        void draw();
        void mousePressed(int x, int y, int button);
        void windowResized(int w, int h);
};

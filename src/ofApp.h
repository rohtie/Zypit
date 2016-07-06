#pragma once

#include "ofMain.h"

#include "clip.h"

class ofApp : public ofBaseApp {
    public:
        ofTrueTypeFont palanquinRegular;

        ofShader shader;

        ofFbo timeline;
        int timelinePos = 0;

        Clip *first;
        Clip *last;
        Clip *playing;
        Clip *defaultClip;

        ofSoundPlayer player;
        ofTexture fftTexture;
        bool isPreprocessing = false;
        float* fftSmoothed;
        vector<float> fftTimeline;

        ofRectangle timelineMarkerRect;
        int  timelineMarker = 0;
        bool isPlaying      = false;
        bool isMovingMarker = false;
        bool isChangingClipTime = false;
        int changingClipTimeBase = 0;
        Clip* changingClip = NULL;
        float orgTime = 0.0;
		
		#ifdef __linux__
        ofFbo exportFbo;
        FILE *exportPipe;
        bool isExporting = false;
		#endif

        ofFbo main;

        void setup();
        void saveClips();

        void update();
        void draw();
        void render(int width, int height);
		
		#ifdef __linux__
        void exportFrame();
		#endif

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y);
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
};

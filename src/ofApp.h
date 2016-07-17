#pragma once

#include "ofMain.h"

#include "clip.h"

class ofApp : public ofBaseApp {
    public:
        ofShader shader;

		#ifndef STANDALONE_PLAYER
		ofTrueTypeFont palanquinRegular;

        ofFbo timeline;
        int timelinePos = 0;
		#endif

        Clip *first;
        Clip *last;
        Clip *playing;
        Clip *defaultClip;

        ofSoundPlayer player;
        ofTexture fftTexture;
		float* fftSmoothed;

		#ifndef STANDALONE_PLAYER
		bool isPlaying = false;
		#else
		bool isPlaying = true;
		#endif
		int  timelineMarker = 0;

		#ifndef STANDALONE_PLAYER
		string newClipName = "";
		bool isAddingNewClip = false;
		bool keepAspectRatio = true;
		bool loopCurrentClip = false;
		Clip *loopingClip = NULL;

        vector<float> fftTimeline;

        ofRectangle timelineMarkerRect;
        bool isMovingMarker = false;
        bool isChangingClipTime = false;
        int changingClipTimeBase = 0;
        Clip* changingClip = NULL;
        float orgTime = 0.0;

		#ifdef __linux__
        ofFbo exportFbo;
		bool isPreprocessing = false;

		FILE *exportPipe;
        bool isExporting = false;
		#endif
		#endif

        ofFbo main;

        void setup();

		#ifndef STANDALONE_PLAYER
        void saveClips();
		void newClip();
		#endif

        void update();
        void draw();
        void render(int width, int height);

		#ifndef STANDALONE_PLAYER
        void screenshot();
		#ifdef __linux__
        void exportFrame();
		#endif
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

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

		int timelineMarker = 0;
		bool isPlaying = false;

		void setup();
		void setupShader(ofShader shader, string filename);

		void update();
		void draw();

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

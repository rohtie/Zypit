#include "ofMain.h"
#include "ofApp.h"

int main( ){
	ofGLWindowSettings settings;

	settings.setGLVersion(3, 2);

	// Standalone player, fullscreen 1080p
	#ifdef STANDALONE_PLAYER
	settings.windowMode = OF_FULLSCREEN;
	settings.width = 1920;
	settings.height = 1080;
	#endif

	ofCreateWindow(settings);

	ofRunApp(new ofApp());
}

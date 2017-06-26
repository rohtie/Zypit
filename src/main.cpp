#include "ofMain.h"
#include "ofApp.h"
#include "ofConfig.h"

int main( ){

	#ifdef STANDALONE_PLAYER
	ofWindowSettings settings;
	settings.width = 300;
	settings.height = 315;
	ofCreateWindow(settings);

	ofGLWindowSettings demoSettings;
	demoSettings.setGLVersion(3, 2);
	demoSettings.windowMode = OF_FULLSCREEN;
	ofRunApp(new ofConfig(&demoSettings));

	ofCreateWindow(demoSettings);
	ofRunApp(new ofApp());
	#else
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
	#endif

}

#include "ofMain.h"
#include "ofApp.h"
#include "ofConfig.h"

int main( ){

	#ifdef STANDALONE_PLAYER
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.width = 300;
	settings.height = 800;
	ofCreateWindow(settings);

	ofGLWindowSettings demoSettings;
	demoSettings.setGLVersion(3, 2);
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

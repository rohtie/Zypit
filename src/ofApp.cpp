#include "pugi/pugixml.hpp"

#include "ofApp.h"
#include "constants.h"
#include "clip.h"

ofApp::ofApp(int _width, int _height) {
    demoWidth = _width;
	demoHeight = _height;
}

void ofApp::setup() {
    ofBackground(BG_COLOR);
    ofSetFrameRate(FPS);

	#ifndef STANDALONE_PLAYER
    palanquinRegular.load("Palanquin-Regular.ttf", TIMELINE_FONT_SIZE);
	#endif

	Clip *current = NULL;

    // Normalize texture coordinates so that they are within 0 to 1 range
    ofDisableArbTex();

    // Allocate texture for the spectrum analysis of the sound playing
    // through the ofSoundPlayer
    fftTexture.allocate(SPECTRUM_WIDTH, 2, GL_RGBA, false);
    player.load("project/song.mp3");

    // Setup clips from XML file
    pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("data/project/clips.xml");

    for (pugi::xml_node clip = doc.first_child(); clip; clip = clip.next_sibling()) {
        string iChannelSrc[4];
        string iChannelFilter[4];

        pugi::xml_node iChannel = clip.first_child();
        for (int i=0; i<4; i++) {
            iChannelSrc[i] = iChannel.attribute("src").value();
            iChannelFilter[i] = iChannel.attribute("filter").value();

            iChannel = iChannel.next_sibling();
        }

		#ifndef STANDALONE_PLAYER
        Clip *newClip = new Clip(
            clip.attribute("src").value(),
            clip.attribute("start").as_int(),
            clip.attribute("length").as_int(),
            clip.attribute("time").as_float(),
            iChannelSrc, iChannelFilter,
            palanquinRegular
        );
		#else
		Clip *newClip = new Clip(
			clip.attribute("src").value(),
			clip.attribute("start").as_int(),
			clip.attribute("length").as_int(),
			clip.attribute("time").as_float(),
			iChannelSrc, iChannelFilter
		);
		#endif

        if (current == NULL) {
            first = newClip;
            current = first;
        }
        else {
            current->right = newClip;
            newClip->left = current;
            current = newClip;
        }

        if (clip.next_sibling() == NULL) {
            last = newClip;
        }
    }

    string iChannel[4];
	#ifndef STANDALONE_PLAYER
    defaultClip = new Clip("../default", 0, 200, 0.0, iChannel, iChannel, palanquinRegular);
	#else
	defaultClip = new Clip("../default", 0, 200, 0.0, iChannel, iChannel);
	#endif

	if (result.status == pugi::status_file_not_found) {
		first = defaultClip;
		last = defaultClip;
	}

	playing = first;

	#ifndef STANDALONE_PLAYER
    // Setup timeline
    timelineMarkerRect.x = -TIMELINE_MARKER_SIZE / 2;
    timelineMarkerRect.width = TIMELINE_MARKER_SIZE;
    timelineMarkerRect.height = TIMELINE_HEIGHT - TIMELINE_CLIP_HEIGHT;

    // Setup video export
    exportFbo.allocate(1920, 1080);
	#endif

    fftSmoothed = new float[8192];
    for (int i = 0; i < 8192; i++){
        fftSmoothed[i] = 0;
    }

	#ifdef STANDALONE_PLAYER
	// Start playing immediately when in standalone player mode
	player.play();
	#endif
}

#ifndef STANDALONE_PLAYER
void ofApp::saveClips() {
    pugi::xml_document doc;

    Clip *current = first;
    while (current != NULL) {
        pugi::xml_node clip = doc.append_child("clip");
        clip.append_attribute("src").set_value(current->src.c_str());
        clip.append_attribute("start").set_value(current->start);
        clip.append_attribute("length").set_value(current->length);
        clip.append_attribute("time").set_value(current->time);

        for (int i=0; i<4; i++) {
            pugi::xml_node iChannel = clip.append_child("iChannel");
            iChannel.append_attribute("src")
                    .set_value(current->iChannelSrc[i].c_str());
            iChannel.append_attribute("filter")
                    .set_value(current->iChannelFilter[i].c_str());
        }

        current = current->right;
    }

    doc.save_file("data/project/clips.xml");
}
void ofApp::newClip() {
	ofFile newFile("newclip.glsl");
	newFile.copyTo("project/" + newClipName + ".glsl");

	int start = last->start + last->length;

	if (last == defaultClip) {
		start = 0;
	}

	// New clips always start with iChannel0 set to the music
	string iChannelSrc[] = {"sound", "", "", ""};
	string iChannelFilter[] = {"", "", "", ""};

	Clip *newClip = new Clip(newClipName, start, 300, 0.0, iChannelSrc, iChannelFilter, palanquinRegular);

	if (last == defaultClip) {
		first = newClip;
		last = newClip;
	}
	else {
		last->right = newClip;
		newClip->left = last;
		last = newClip;
	}

	newClipName = "";
}
#endif

void ofApp::update() {
    // Load spectrum analysis into texture
    // TODO: Implement this in the same way as done in the web audio API
    //       for getFloatFrequencyData: https://webaudio.github.io/web-audio-api/#fft-windowing-and-smoothing-over-time
    #ifndef STANDALONE_PLAYER
    if (!isExporting) {
    #endif
        float * val = ofSoundGetSpectrum(512);
        for (int i = 0; i < SPECTRUM_WIDTH; i++){
            // Let signal dimminish over time to make the visual more pronounced
            fftSmoothed[i] *= 0.97;

            // Refresh signal if it is more powerful than the current signal
            float fft = ofClamp(val[i] * 10.0, 0.0, 1.0);
            if (fftSmoothed[i] < fft) {
                fftSmoothed[i] = fft;
            }

            fftSmoothed[i + SPECTRUM_WIDTH] = fftSmoothed[i];

            #ifndef STANDALONE_PLAYER
            if (isPreprocessing) {
                fftTimeline.push_back(fftSmoothed[i]);
            }
            #endif
        }
    #ifndef STANDALONE_PLAYER
    } else if (isExporting) {
        for (int i=0; i<SPECTRUM_WIDTH; i++) {
            fftSmoothed[i] = fftTimeline[i + timelineMarker * SPECTRUM_WIDTH];
            fftSmoothed[i + SPECTRUM_WIDTH] = fftSmoothed[i];
        }
    }
    #endif

	fftTexture.loadData(fftSmoothed, SPECTRUM_WIDTH, 2, GL_RED);

	Clip *current = first;

	#ifndef STANDALONE_PLAYER
    // Scroll timeline when within timeline area.
    if (mouseY > ofGetHeight() - TIMELINE_HEIGHT) {
        if (mouseX < TIMELINE_SCOLLING_AREA) {
            timelinePos -= TIMELINE_SCOLLING_SPEED;
        }
        else if (mouseX > ofGetWidth() - TIMELINE_SCOLLING_AREA) {
            timelinePos += TIMELINE_SCOLLING_SPEED;
        }
    }

    int x = mouseX + timelinePos;
    int y = mouseY - ofGetHeight() - TIMELINE_HEIGHT;

    // Update clips
    while (current != NULL) {
        int res = current->update(x, y);

        // After swapping, the current clip might have become the first or last clip in the list
        if (res == 1) {
            if (current->left == NULL) {
                first = current;
            }
            else if (current->left->left == NULL) {
                first = current->left;
            }

            if (current->right == NULL) {
                last = current;
            }
            else if (current->right->right == NULL) {
                last = current->right;
            }
        }

        current = current->right;
    }

    if (isMovingMarker) {
        timelineMarker = x;

		if (timelineMarker < 0) {
			timelineMarker = 0;
		}
		else if (timelineMarker > last->start + last->length) {
			timelineMarker = last->start + last->length;
		}

        player.setPositionMS((int) ((timelineMarker / FPS) * 1000));
    }
	#endif

    // Get current playing clip
    if (isPlaying) {
        #ifndef STANDALONE_PLAYER
        if (isPreprocessing || isExporting) {
            timelineMarker += 1;
        }
        else {
        #endif
		  timelineMarker = (player.getPositionMS() / 1000.0) * FPS;
        #ifndef STANDALONE_PLAYER
        }
        #endif

		#ifndef STANDALONE_PLAYER
		if (loopCurrentClip) {
			if (timelineMarker > loopingClip->start + loopingClip->length) {
				timelineMarker = loopingClip->start;
				player.setPositionMS((int)((timelineMarker / FPS) * 1000));
			}
		}
        else if (timelineMarker > last->start + last->length) {
		#else
		if (timelineMarker > last->start + last->length || !player.isPlaying()) {
		#endif
			#ifndef STANDALONE_PLAYER
            // We have reached the end of the timeline
            timelineMarker = 0;
            player.setPositionMS(0);
			#else
			// Demo is finished, close it
			ofExit();
			#endif

            #ifndef STANDALONE_PLAYER
            if (isPreprocessing) {
                isPreprocessing = false;
                isExporting = true;

                // Open up a pipe to ffmpeg so that we can send PNG images to it
                // which will be sequenced into a video file
                stringstream ffmpeg;
                ffmpeg <<
                    // thread_queue_size is raised to avoid discarded frames
                    // image2pipe makes ffmpeg accept images through the pipe
                    "ffmpeg -thread_queue_size 512 -y -f image2pipe" <<

                    // Set resolution
                    " -s " << EXPORT_WIDTH << "x" << EXPORT_HEIGHT <<

                    // Set video input source to pipe at FPS rate
                    " -r " << (int) FPS << " -i -" <<

                    // Set audio input source to mp3 file at FPS rate
                    " -r " << (int) FPS << " -i data/project/song.mp3" <<

                    // Set input image type to png and output video to x264
                    " -vcodec png -c:v libx264" <<

                    // ffmpeg has to be set to experimental mode to allow mp3
                    // file to merged into the video output
                    " -c:a aac -strict experimental" <<

                    // Set output video at FPS rate
                    // 18 crf ensures a visually lossless quality
                    " -r " << (int) FPS << " -crf 18 -vn out.mp4";

                exportPipe = _popen(ffmpeg.str().c_str(), "w");
                player.stop();
            }
            else if (isExporting) {
                // We are done exporting
                isExporting = false;
                _pclose(exportPipe);
                fftTimeline.clear();
                isPlaying = false;
                player.stop();
            }
            #endif
        }
    }

	#ifndef STANDALONE_PLAYER
    timelineMarkerRect.x = timelineMarker - TIMELINE_MARKER_SIZE / 2;
	#endif

    playing = defaultClip;

    current = first;
    while (current != NULL) {
        if (timelineMarker >= current->start && timelineMarker <= current->start + current->length) {
            playing = current;
        }
        current = current->right;
    }
}

void ofApp::draw() {
    int width = ofGetWidth();
    int height = ofGetHeight();

    if (showUI) {
        height -= TIMELINE_HEIGHT;
    }

    // MAIN SCREEN
	#ifndef STANDALONE_PLAYER
    if (!isPreprocessing && !isExporting) {
		int x = 0;
		int y = 0;

		int shaderHeight = height;
		int shaderWidth = width;

		if (keepAspectRatio) {
			if (shaderHeight > shaderWidth || shaderHeight * HEIGHT_TO_ASPECT_RATIO > shaderWidth) {
				y += shaderHeight / 2;
				shaderHeight = shaderWidth * WIDTH_TO_ASPECT_RATIO;
				y -= shaderHeight / 2;
			}
			else if (shaderWidth > shaderHeight) {
				x += shaderWidth / 2;
				shaderWidth = shaderHeight * HEIGHT_TO_ASPECT_RATIO;
				x -= shaderWidth / 2;
			}

			main.allocate(shaderWidth, shaderHeight);
			main.begin();
			render(shaderWidth, shaderHeight);
			main.end();
			main.draw(x, y);
		}
		else {
	#endif
            #ifndef STANDALONE_PLAYER
            main.allocate(width, height);
            main.begin();
            render(width, height);
            main.end();
			main.draw(0, 0);
            #else
            main.allocate(demoWidth, demoHeight);
            main.begin();
            render(demoWidth, demoHeight);
            main.end();
            main.draw(0, 0, width, height);
            #endif
	#ifndef STANDALONE_PLAYER
		}
	#endif
	}

    // Export
    exportFrame();

	#ifndef STANDALONE_PLAYER
    if (showUI) {
        // TIMELINE
        timeline.allocate(last->start + last->length, TIMELINE_HEIGHT);
        timeline.begin();
            ofBackground(TIMELINE_BG_COLOR);

            Clip *current = first;
            while (current != NULL) {
                current->draw();
                current = current->right;
            }

            ofSetColor(255, 0, 0);
            ofDrawLine(timelineMarker, 0, timelineMarker, TIMELINE_HEIGHT);
            ofDrawRectangle(timelineMarkerRect);

        timeline.end();
        timeline.draw(-timelinePos, height);

        // Transparent black box so that white text can be visible at all times
        ofSetColor(0, 0, 0, INFORBAR_ALPHA);
        ofDrawRectangle(
            0, ofGetHeight() - TIMELINE_HEIGHT - INFOBAR_HEIGHT,
            ofGetWidth(), INFOBAR_HEIGHT
        );

        ofSetColor(INFORBAR_DATA_COLOR);

        // Draw FPS
        std::stringstream fps;
        fps << round(ofGetFrameRate());
        palanquinRegular.drawString(
            fps.str(),
            TIMELINE_FONT_SIZE,
            height - INFOBAR_HEIGHT / 2 + TIMELINE_FONT_SIZE / 2
        );

        std::stringstream time;
        time << roundf((timelineMarker / FPS) * 100.0) / 100.0;
        palanquinRegular.drawString(
            time.str(),
            TIMELINE_FONT_SIZE * 3,
            height - INFOBAR_HEIGHT / 2 + TIMELINE_FONT_SIZE / 2
        );

        if (isPlaying) {
            if (isPreprocessing) {
                palanquinRegular.drawString(
                    "Baking FFT...",
                    TIMELINE_FONT_SIZE * 8,
                    ofGetHeight() - TIMELINE_HEIGHT - INFOBAR_HEIGHT / 2 + TIMELINE_FONT_SIZE / 2
                );
            }
            else if (isExporting) {
                palanquinRegular.drawString(
                    "Rendering...",
                    TIMELINE_FONT_SIZE * 8,
                    ofGetHeight() - TIMELINE_HEIGHT - INFOBAR_HEIGHT / 2 + TIMELINE_FONT_SIZE / 2
                );
            }
        }

    	if (isAddingNewClip) {
    		palanquinRegular.drawString(
    			"New clip name: " + newClipName,
    			TIMELINE_FONT_SIZE * 8,
    			ofGetHeight() - TIMELINE_HEIGHT - INFOBAR_HEIGHT / 2 + TIMELINE_FONT_SIZE / 2
    		);
    	}

        // Visualize spectrum
        for (int i = 0; i < SPECTRUM_WIDTH; i++) {
            float magnitude = fftSmoothed[i] * INFOBAR_HEIGHT;

            ofDrawRectangle(
                ofGetWidth() - SPECTRUM_WIDTH + i,
                ofGetHeight() - TIMELINE_HEIGHT - 1 - magnitude,
                1, magnitude);
        }
    }
	#endif
}

void ofApp::render(int width, int height) {
    ofSetColor(255);

    float currentTime = timelineMarker / FPS;
    float startTime = playing->start / FPS;
    float requestTime = playing->time;

    playing->shader.begin();
        playing->shader.setUniform1f("iGlobalTime", currentTime - (startTime - requestTime));
        playing->shader.setUniform2f("iResolution", width, height);

        for (int i=0; i<4; i++) {
            if (playing->iChannel[i].isAllocated()) {
                playing->shader.setUniformTexture("iChannel" + ofToString(i), playing->iChannel[i], i);
            }
            else if (playing->soundChannel == i) {
                playing->shader.setUniformTexture("iChannel" + ofToString(playing->soundChannel), fftTexture, i);
            }
        }

        ofDrawRectangle(0, 0, width, height);
    playing->shader.end();
}

#ifndef STANDALONE_PLAYER
void ofApp::screenshot() {
    ofFbo screenshot;
    screenshot.allocate(SCREENSHOT_WIDTH, SCREENSHOT_HEIGHT);

    screenshot.begin();
        render(SCREENSHOT_WIDTH, SCREENSHOT_HEIGHT);
    screenshot.end();

    ofPixels pixels;
    screenshot.readToPixels(pixels);

    ofImage image;
    image.setFromPixels(pixels);
    image.save("screenshot.png");
}

void ofApp::exportFrame() {
    if (isExporting && isPlaying) {
        exportFbo.begin();
            render(exportFbo.getWidth(), exportFbo.getHeight());
        exportFbo.end();

        ofPixels pixels;
        exportFbo.readToPixels(pixels);

        ofImage image;
        image.setFromPixels(pixels);

        ofBuffer buffer;
        ofSaveImage(image.getPixels(), buffer);

        fwrite(buffer.getData(), buffer.size(), 1, exportPipe);
    }
}
#endif

void ofApp::keyPressed(int key) {
	#ifndef STANDALONE_PLAYER
	if (isAddingNewClip) {
		// Remove last character when using backspace
		if (key == OF_KEY_BACKSPACE && newClipName.size() > 0) {
			newClipName.pop_back();
		}
		// Return key finishes writing the clip name
		else if (key == OF_KEY_RETURN) {
			newClip();
			isAddingNewClip = false;
		}
		// Only accept ASCII alphanumeric keys
		else if (key > -1 && key < 256 && (isalpha(key) || key == ' ' || key == '-' || key == '_')) {
			newClipName += key;
		}

		return;
	}

    if (key == ' ') {
        if (isPreprocessing || isExporting) {
            player.stop();
            isPlaying = false;
            fftTimeline.clear();
        }

        if (isPreprocessing) {
            isPreprocessing = false;
        }
        else if (isExporting) {
            isExporting = false;
            _pclose(exportPipe);
        }
        else {
            if (isPlaying) {
				player.stop();
            }
            else {
				player.play();
				player.setPositionMS((int)((timelineMarker / FPS) * 1000));
            }
			isPlaying = !isPlaying;
        }
    }
    else if (isPreprocessing || isExporting) {
        // Prevent the other keys from working while preprocessing or exporting
        return;
    }
    else if (key == 'e') {
        isPreprocessing = true;
        timelineMarker = 0;
        isPlaying = true;

        player.setPositionMS(0);
        player.play();
    }
	// Change timing of clip
    else if (key == 't') {
        // Assume that the current playing clip is the one we want to change.
        isChangingClipTime = !isChangingClipTime;

        changingClipTimeBase = mouseX;
        orgTime = playing->time;
        changingClip = playing;
    }

    // Openframeworks does not support checking of CTRL directly, so we are
    // using s instead of ctrl + s to save.
    else if (key == 's') {
        saveClips();
    }
	// Create new clip
	else if (key == 'n') {
		isAddingNewClip = true;
	}
	// Create new clip
	else if (key == 'f') {
		keepAspectRatio = !keepAspectRatio;
	}
	// Loop current clip
	else if (key == 'l') {
		loopCurrentClip = !loopCurrentClip;
		loopingClip = playing;
	}
	// Mute music
	else if (key == 'm') {
		if (player.getVolume() == 0.0f) {
			player.setVolume(1.0f);
		}
		else {
			player.setVolume(0.0f);
		}
	}
    // Toggle UI
    else if (key == OF_KEY_F10) {
        showUI = !showUI;
    }
    // Toggle fullscreen
    else if (key == OF_KEY_F11) {
        isFullscreen = !isFullscreen;
        ofSetFullscreen(isFullscreen);
    }
    // Screenshot
    else if (key == OF_KEY_F12) {
        screenshot();
    }
	// Delete clip
	else if (key == 'x') {
		// There has to be at least one clip left
		if (playing->left == NULL && playing->right == NULL) {
			return;
		}

		if (playing == first) {
			Clip *newFirstClip = playing->right;
			newFirstClip->left = NULL;

			first = newFirstClip;
		}
		else if (playing == last) {
			Clip *newLastClip = playing->left;
			newLastClip->right = NULL;

			last = newLastClip;
		}
		else {
			Clip *left = playing->left;
			Clip *right = playing->right;

			left->right = right;
			right->left = left;

			playing = left;
		}
	}
	#endif
}

void ofApp::keyReleased(int key) {
}

void ofApp::mouseMoved(int x, int y) {
	#ifndef STANDALONE_PLAYER
    if (isChangingClipTime) {
        // If the clip has been changed, we disable time changing to prevent
        // unintuitive time changes on other clips.
        if (changingClip != playing) {
            isChangingClipTime = false;
            return;
        }

        playing->time = orgTime - (changingClipTimeBase - x) / FPS;
    }
	#endif
}

void ofApp::mouseDragged(int x, int y, int button) {
}

void ofApp::mousePressed(int x, int y, int button) {
	#ifndef STANDALONE_PLAYER
    // Make sure we click on the correct part of the timeline
    x += timelinePos;
    y -= ofGetHeight() - TIMELINE_HEIGHT;

    if (y < TIMELINE_HEIGHT - TIMELINE_CLIP_HEIGHT) {
        isMovingMarker = true;
        return;
    }

    Clip *current = first;
    while (current != NULL) {
        current->inside(x, y);
        current = current->right;
    }
	#endif
}

void ofApp::mouseReleased(int x, int y, int button) {
	#ifndef STANDALONE_PLAYER
    // Make sure we click on the correct part of the timeline
    x += timelinePos;
    y -= ofGetHeight() - TIMELINE_HEIGHT;

    if (isMovingMarker) {
        isMovingMarker = false;
    }

    Clip *current = first;
    while (current != NULL) {
        current->deSelect();
        current = current->right;
    }
	#endif
}

void ofApp::windowResized(int w, int h) {
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
}
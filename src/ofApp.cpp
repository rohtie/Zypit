#include "pugi/pugixml.hpp"

#include "ofApp.h"
#include "constants.h"
#include "clip.h"

void ofApp::setup() {
    ofBackground(BG_COLOR);

    palanquinRegular.load("Palanquin-Regular.ttf", TIMELINE_FONT_SIZE);

    // Normalize texture coordinates so that they are within 0 to 1 range
    ofDisableArbTex();

    // Allocate texture for the spectrum analysis of the sound playing
    // through the ofSoundPlayer
    fftTexture.allocate(512, 1, GL_RGBA, false);
    player.load("song.mp3");

    // Setup clips from XML file
    pugi::xml_document doc;
    doc.load_file("data/clips.xml");

    Clip *current = NULL;

    for (pugi::xml_node clip = doc.first_child(); clip; clip = clip.next_sibling()) {
        string iChannel[] = {
            clip.attribute("iChannel0").value(),
            clip.attribute("iChannel1").value(),
            clip.attribute("iChannel2").value(),
            clip.attribute("iChannel3").value()
        };

        Clip *newClip = new Clip(
            clip.attribute("src").value(),
            clip.attribute("start").as_int(),
            clip.attribute("length").as_int(),
            clip.attribute("time").as_float(),
            iChannel,
            palanquinRegular
        );

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
    defaultClip = new Clip("default", 0, 200, 0.0, iChannel, palanquinRegular);
    playing = first;

    // Setup timeline
    timelineMarkerRect.x = -TIMELINE_MARKER_SIZE / 2;
    timelineMarkerRect.width = TIMELINE_MARKER_SIZE;
    timelineMarkerRect.height = TIMELINE_HEIGHT - TIMELINE_CLIP_HEIGHT;

    // Setup video export
    exportFbo.allocate(1280, 720);
}

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
            clip.append_attribute(("iChannel" + ofToString(i)).c_str())
                .set_value(current->iChannelNames[i].c_str());
        }

        current = current->right;
    }

    doc.save_file("data/clips.xml");
}

void ofApp::update() {
    // Load spectrum analysis into texture
    fftTexture.loadData(ofSoundGetSpectrum(512), 512, 1, GL_LUMINANCE);

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
    Clip *current = first;
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
        player.setPositionMS((int) ((timelineMarker / FPS) * 1000));
    }

    // Get current playing clip
    if (isPlaying) {
        timelineMarker += 1;
        timelineMarker = timelineMarker % (last->start + last->length);
    }

    timelineMarkerRect.x = timelineMarker - TIMELINE_MARKER_SIZE / 2;

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
    int height = ofGetHeight() - TIMELINE_HEIGHT;

    // MAIN SCREEN
    main.allocate(width, height);
    main.begin();
    render(width, height);
    main.end();
    main.draw(0, 0);

    // Export
    exportFrame();

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

    if (isExportMode && isPlaying) {
        palanquinRegular.drawString(
            "Rendering...",
            TIMELINE_FONT_SIZE * 3,
            ofGetHeight() - TIMELINE_HEIGHT - INFOBAR_HEIGHT / 2 + TIMELINE_FONT_SIZE / 2
        );
    }

    // Visualize spectrum
    float* buffer = ofSoundGetSpectrum(512);
    for (int i = 0; i < 512; i++) {
        ofDrawRectangle(
            ofGetWidth() - 512 + i,
            ofGetHeight() - TIMELINE_HEIGHT - 1 - buffer[i] * INFOBAR_HEIGHT,
            1, buffer[i] * INFOBAR_HEIGHT);
    }
}

void ofApp::render(int width, int height) {
    ofSetColor(255);

    playing->shader.begin();
        playing->shader.setUniform1f("iGlobalTime", timelineMarker / FPS - playing->time);
        playing->shader.setUniform2f("iResolution", width, height);

        for (int i=0; i<4; i++) {
            if (playing->iChannel[i].isAllocated()) {
                playing->shader.setUniformTexture("iChannel" + ofToString(i), playing->iChannel[i], i);
            }

            if (playing->soundChannel > -1) {
                playing->shader.setUniformTexture("iChannel" + ofToString(playing->soundChannel), fftTexture, i);
            }
        }

        ofDrawRectangle(0, 0, width, height);
    playing->shader.end();
}

void ofApp::exportFrame() {
    if (isExportMode && isPlaying) {
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

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        isPlaying = !isPlaying;

        if (isPlaying) {
            player.play();
        }
        else {
            player.stop();
        }

        if (isExportMode) {
            if (isPlaying) {
                // Open up a pipe to ffmpeg so that we can send PNG images to it
                // which will be sequenced into a video file
                stringstream ffmpeg;
                ffmpeg <<
                    "ffmpeg -y -f image2pipe -s " <<
                    exportFbo.getWidth() << "x" << exportFbo.getHeight() <<
                    " -i - -vcodec png -c:v libx264" <<
                    " -r 60 -crf 25 out.mp4";

                exportPipe = popen(ffmpeg.str().c_str(), "w");
            }
            else {
                pclose(exportPipe);
            }
        }
    }
    else if (key == 'e') {
        isExportMode = !isExportMode;
    }
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
}

void ofApp::keyReleased(int key) {
}

void ofApp::mouseMoved(int x, int y) {
    if (isChangingClipTime) {
        // If the clip has been changed, we disable time changing to prevent
        // unintuitive time changes on other clips.
        if (changingClip != playing) {
            isChangingClipTime = false;
            return;
        }

        playing->time = orgTime + (changingClipTimeBase - x) / FPS;
    }
}

void ofApp::mouseDragged(int x, int y, int button) {
}

void ofApp::mousePressed(int x, int y, int button) {
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
}

void ofApp::mouseReleased(int x, int y, int button) {
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
}

void ofApp::windowResized(int w, int h) {
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
}
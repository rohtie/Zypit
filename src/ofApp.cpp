#include "pugi/pugixml.hpp"

#include "ofApp.h"
#include "constants.h"
#include "clip.h"

void ofApp::setup() {
    ofBackground(BG_COLOR);

    palanquinRegular.load("Palanquin-Regular.ttf", TIMELINE_FONT_SIZE);

    // Normalize texture coordinates so that they are within 0 to 1 range
    ofDisableArbTex();

    // Setup clips from XML file
    pugi::xml_document doc;
    doc.load_file("data/clips.xml");

    Clip *current = NULL;

    for (pugi::xml_node clip = doc.first_child(); clip; clip = clip.next_sibling()) {
        Clip *newClip = new Clip(clip.attribute("start").as_int(),
                             clip.attribute("length").as_int(),
                             clip.attribute("src").value(),
                             palanquinRegular);

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

    defaultClip = new Clip(0, 200, "default", palanquinRegular);
    playing = first;

    // Setup timeline
    timelineMarkerRect.x = -TIMELINE_MARKER_SIZE / 2;
    timelineMarkerRect.width = TIMELINE_MARKER_SIZE;
    timelineMarkerRect.height = TIMELINE_HEIGHT - TIMELINE_CLIP_HEIGHT;
}

void ofApp::saveClips() {
    pugi::xml_document doc;

    Clip *current = first;
    while (current != NULL) {
        pugi::xml_node clip = doc.append_child("clip");
        clip.append_attribute("src").set_value(current->src.c_str());
        clip.append_attribute("start").set_value(current->start);
        clip.append_attribute("length").set_value(current->length);

        current = current->right;
    }

    doc.save_file("data/clips.xml");

}

void ofApp::update() {
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
    ofSetColor(255);
    playing->shader.begin();
        playing->shader.setUniform1f("iGlobalTime", timelineMarker / 60.0f);
        playing->shader.setUniform2f("iResolution", width, height);
        ofDrawRectangle(0, 0, width, height);
    playing->shader.end();

    std::stringstream fps;
    fps << round(ofGetFrameRate());
    palanquinRegular.drawString(fps.str(), TIMELINE_FONT_SIZE, height - TIMELINE_FONT_SIZE);

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
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        isPlaying = !isPlaying;
    }

    // Openframeworks does not support checking of CTRL directly, so we are
    // using s instead of ctrl + s ATM to save.
    else if (key == 's') {
        saveClips();
    }
}

void ofApp::keyReleased(int key) {
}

void ofApp::mouseMoved(int x, int y) {
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
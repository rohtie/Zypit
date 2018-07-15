#pragma once

#include "ofMain.h"
#include "constants.h"

#include "Poco/Timestamp.h"
#include "Poco/File.h"

class Shader {
    public:
        string src;
        ofShader shader;

        ofTexture iChannel[4];
        string iChannelSrc[4];
        string iChannelFilter[4];

        int soundChannel = -1;

        #ifndef STANDALONE_PLAYER
        Poco::Timestamp lastTimestamp;
        #endif

    Shader(string _src, string* _iChannelSrc, string* _iChannelFilter) {
        src = _src;
        setupTextureChannels(_iChannelSrc, _iChannelFilter);
        setupShader();
    }

    void setupTextureChannels(string* _iChannelSrc, string* _iChannelFilter) {
        for (int i=0; i<4; i++) {
            iChannelSrc[i] = _iChannelSrc[i];
            iChannelFilter[i] = _iChannelFilter[i];

            if (!iChannelSrc[i].empty()) {
                if (iChannelSrc[i] == "sound") {
                    soundChannel = i;
                }
                else {
                    // TODO: Support grayscale images so that the image passed
                    // to the shader does not just fill the red channel.
                    ofLoadImage(iChannel[i], "project/" + iChannelSrc[i]);
                    iChannel[i].setTextureWrap(GL_REPEAT, GL_REPEAT);

                    if (iChannelFilter[i] == "nearest") {
                        iChannel[i].setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
                    }
                    else if (iChannelFilter[i] == "linear") {
                        iChannel[i].setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
                    }
                    // Assume that we always want mipmap if not specified otherwise
                    else {
                        iChannel[i].enableMipmap();
                        iChannel[i].generateMipmap();
                        iChannel[i].setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
                    }
                }
            }
        }
    }

    void setupShader() {
        shader.setupShaderFromFile(GL_VERTEX_SHADER, "vertex.glsl");

        #ifndef STANDALONE_PLAYER
        Poco::File pocoShaderFile("data/project/" + src + ".glsl");
        lastTimestamp = pocoShaderFile.getLastModified();
        #endif

        ifstream shaderFile("data/project/" + src + ".glsl");
        stringstream shaderSource;

        if (shaderFile) {
            shaderSource
                << SHADER_HEADER
                << shaderFile.rdbuf()
                << SHADER_FOOTER;

            shaderFile.close();
        }

        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderSource.str());
        shader.bindDefaults();
        shader.linkProgram();
    }

    #ifndef STANDALONE_PLAYER
    void reloadShader() {
        setupShader();
    }

    void update() {
        // Reload shader if shader file has been modified
        // TODO: Use Poco::DirectoryWatcher when they fix the thread crashing
        Poco::File shaderFile("data/project/" + src + ".glsl");
        Poco::Timestamp timestamp = shaderFile.getLastModified();
        if (timestamp != lastTimestamp) {
            lastTimestamp = timestamp;
            reloadShader();
        }
    }
    #endif
};

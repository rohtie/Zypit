#pragma once

#if !defined(CONSTANT_VALUES)
#define CONSTANT_VALUES 1
const float FPS = 60.0;

const int BG_COLOR = 60;

const int TIMELINE_HEIGHT = 50;
const int TIMELINE_CLIP_HEIGHT = 40;
const int TIMELINE_SCOLLING_AREA = 50;
const int TIMELINE_SCOLLING_SPEED = 10;
const int TIMELINE_EXPAND_AREA = 10;
const int TIMELINE_MARKER_SIZE = 20;

const int TIMELINE_FONT_SIZE = 15;
const int TIMELINE_BG_COLOR = 72;
const int TIMELINE_CLIP_COLOR = 128;
const int TIMELINE_FONT_COLOR = 200;

const int INFOBAR_HEIGHT = 32;
const int INFORBAR_ALPHA = 200;
const int INFORBAR_DATA_COLOR = 255;

// To prevent dead frequencies from showing on the spectrum, the width is cropped
// to 400, which will contain all the live frequencies.
const int SPECTRUM_WIDTH = 400;

const string SHADER_HEADER = "#pragma include <header.glsl>\n\n";
const string SHADER_FOOTER = "\n#pragma include <footer.glsl>\n";
#endif

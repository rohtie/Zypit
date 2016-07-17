Zypit
====
Demo tool for fragment shaders.

.. image:: http://i.imgur.com/9C8ycBV.png

Features
--------

* Simple video-like editor with a single track
* Shadertoy shaders can be plugged in directly
* Texture support similar to shadertoy with filter choices
* Music is attached to shaders as a FFT spectrum analysis texture
* Live reload of shaders when saved
* Stripped down player for release of demo
* Export demo to mp4 video in any resolution (Linux only)

Usage
-----

======  ========
Hotkey  Function
======  ========
Space   Play project.
N       Create new clip. If you give it the same name as a shader in your project folder, it will use that shader instead of creating a     new one.
X       Delete the clip that you are currently viewing.
S       Save project.
F       Toggles whether the shader view should be constrained to 16:9 aspect ratio or not.
L       Loop current playing clip.
M       Mute music.
E       Export project to a 1920x1080 HD mp4 video file (Linux only).
F12     Take a screenshot with 3840x2160 4K resolution.
Esc     Quit.
======  ========

Setup
-----

Download openFrameworks. Create a new app with the project generator.

In the generated app directory execute the following:

::

    git init
    git remote add origin git@github.com:rohtie/Zypit.git
    git fetch
    git reset origin/master
    git checkout .

If you're running the binary from an IDE, make sure the current working directory is set to `./bin`.

If you want to try out an example project, you can clone `Zypit Tech Demo <https://github.com/rohtie/Zypit-tech-demo>`_ to the ``bin/data/project/`` directory. Like this: ``git clone git@github.com:rohtie/Zypit-tech-demo.git bin/data/project``

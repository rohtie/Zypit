Zypit
====
Demo tool for fragment shaders.

Features:

* Simple video-like editor with a single track
* Shadertoy shaders can be plugged in directly
* Music is attached to shaders as a FFT texture
* Live reload of shaders when saved
* Stripped down player for release of demo
* Export demo to mp4 video in any resolution (POSIX only, windows support planned)

.. image:: http://i.imgur.com/x1gSepQ.png

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


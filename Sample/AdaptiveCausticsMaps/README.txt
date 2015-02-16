**************************************************************************
*             Adaptive Caustic Maps Using Deferred Shading               *
*   (Computer Graphics Forum 28(2) & Proceedings of Eurographics 2009)   *
*                     Chris Wyman and Greg Nichols                       *
*                          University of Iowa                            *
*    More Information: http://www.cs.uiowa.edu/~cwyman/publications/     *
**************************************************************************

This directory contains executables and code for a demo for our paper
entitled:  "Adaptive Caustic Maps Using Deferred Shading."

This demo requires:
   1) GLUT (a .dll is included)
   2) GLEW (a .dll is included)
   3) A DirectX-10 class graphics card under Windows XP or Vista
   4) An nVidia card, due to the use of NV_transform_feedback.  The OpenGL 
      extension EXT_transform_feedback should be sufficient, but I do not
      have access to a card/driver pair that implements this.

The demo was developed, at various times, on GeForce 8800 GTX, 9800 GTX, 
and GTX 280 graphics cards.  Our current nVidia driver is version 180.43,
but the most recent driver should not be required.  The demo does not 
scale on multiple-GPU configurations.

PLEASE NOTE:  Video memory requirements for this demo are artificially
high (due to the ability to tweak parameters and switch freely between 
very different rendering modes, each with different video buffers).  Our 
experience is that a card with at least 768 MB memory (per GPU core) is 
required to avoid painful swapping of video memory.  Implementations that 
focus on a single preferred approach to caustic rendering and avoid user-
tweakable parameters can reduce memory requirements considerably.

Please be aware that to keep the demo relatively simple to use (and
understand the code), not all parameters are fully tweakable.  To maintain 
full configurability and the ability to easily compare between rendering
parameters and prior work, our research prototype has 15 different 
"rendering modes."  We expose only six of these.  We believe these six
should be obviously different.  

The project file is for Microsoft Visual Studio .NET 2008. 

The codebase relies on a "standard" framework I have used for most of my 
recent OpenGL-based research.  The idea is to use a scenegraph-like 
construct that hides most of the rendering details from me when I want to 
render the whose scene at once, but still lets me easily pick out
individual objects to render as glass, with caustics, or casting volumetric
shadows.

Most of the code (inside the directories DataTypes, Interface, Materials,
Objects, Scenes, and Utils) is thus framework code that is not particularly
relevant to this demo.  While this code is mostly straightforward and
"self-commenting," I have not spent time making sure it is particularly
legible.

Most of the interesting stuff goes on inside the directory 
"RenderingTechniques/" which includes all the code for the various 
"rendering modes" mentiond above.  For example, One mode (in 
deferredRefraction.cpp) only draws the scene with a glass object rendered 
via deferred refraction.  The code inside this directory is quite well
commented.  The "sceneLoader.cpp" code in the main directory keeps 
everything together (main() routine, display callback, etc).

Shaders are located in "bin/shaders/" and a README in that directory
describes which are particularly relevant to this demo.  


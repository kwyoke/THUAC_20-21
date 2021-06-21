# Assignment 1 Report

Yoke Kai Wen 2020280598

## Files and directory structure

Main script: main1.cpp
Shader files: main1.vert.glsl, main1.frag.glsl
Object file: eight.uniform.obj
Object loader header file: obj3dmodel.h
Other header files: camera.h, shader.h

Note that header files should be placed in the correct Include directory. Other glfw, glad, glm standard libraries should be included as in previous tutorials.


## Usage

Use `V` to switch among the four display modes.

Use `C` to change the color of points and the wireframe. 

Use `W`, `S`, `A`, `D`, `Z`, and `X` to translate the model along the three axes.

Use `U`, `H`, `J`, `K`, `N`, and `M` to rotate the model about the three axes.


## Design

### Loading 3D mesh model

The 3D mesh model in the 'obj3dmodel.h' file is parsed and loaded with the 'obj3dmodel.h' header file. The vertices and faces are stored in vectors of vertex and face structs. In the main script, I then convert the vertice and face vectors into an array of floats - each face is represented by 18 floats, with the first three floats representing the position of the first vertex, the second three floats representing the intermediate 'colour' of the first vertex, and so on for the second and third vertex of the tranglular face. Hence, the array of floats have a total of 18*(number of faces) floats.

### Displaying four modes
The four modes of display, wireframe mode, vertex mode, face mode and face and edge mode can be controlled by pressing the `V` key which increments the 'currentMode' variable and determines which mode to display in a switch statement. The display mode can be configured using the OpenGL function `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)`, `glPolygonMode(GL_FRONT_AND_BACK, GL_POINT)` and `glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)`. 

However, for the face and edge mode, we need to draw the array twice, first we draw the face mode, and then we draw the line mode on top of the face mode to achieve face and edge display.

The face mode is also displayed such that the colour of each face is different. This is achieved by first setting an intermediate colour vector for each vertex according to the coordinates of the centre position of the triangle face that the vertex belongs to (i.e. all three vertices of the same face have the same intermediate colour vector, and different faces will have different centre coordinates). We want adjacent faces to have different colours, so we cannot correlate the colour of the face to its centre coordinates. Instead, we take the value of the centre coordinates from the 4th decimal place onwards, so that the colour of each face will be random, and thus it will look like the colour of each face is different.

### Rotation and translation
Rotation and translation is controlled by the various direction keys. Pressing each key alters the orientation or position of the model by calculating the distance moved in the defined direction based on the configured speed. Global variables store the current position and orientation, which is then updated in the `move_model()` function according to the key press. The updated position and orientation variables are then used to compute the translation and rotation matrices using the `glm::rotate` and `glm::translate` functions, and applied to the model.

### Changing colours under wireframe mode
The colour of the wireframe is determined by global colour variables. Whenever the 'C' key is pressed, the def_red, def_green and def_blue variables are incremented, and the updated colours are passed into the fragment shader for colouring the wireframe and the vertex points.
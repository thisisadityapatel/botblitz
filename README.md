### Description

Botblitz is a 3D minigame designed in OpenGL where players are tasked with shooting waves of robots (Optimus Prime), players must hit all robots before they reach the finish line (behind the cannon). If a robot reaches the end it dies, once all robots are dead the level resets.

https://github.com/user-attachments/assets/37946ad6-0d0c-4a24-9611-c754dddf8018

https://github.com/user-attachments/assets/6d1b335e-ca60-440b-9557-83b02721017d

There are three levels for the game, with each level the robots become faster making it difficult to hunt them down using the defensive cannon.

Click `7` for `Level 1` - Easy (Default)  
Click `8` for `Level 2` - Medium  
Click `9` for `Level 3` - Hard

### Compiling Information

GLEW installation

```bash
brew install cmake
```

```bash
brew install glew
```

freeglut installation

```bash
brew install freeglut
```

glfw installation

```bash
brew install glfw
```

After following the above run:

```bash
g++ -o main main.cpp \
-I/opt/homebrew/Cellar/glew/2.2.0_1/include \
-I/opt/homebrew/Cellar/freeglut/3.6.0/include \
-I/opt/homebrew/Cellar/glfw/3.4/include \
-L/opt/homebrew/Cellar/glew/2.2.0_1/lib \
-L/opt/homebrew/Cellar/freeglut/3.6.0/lib \
-L/opt/homebrew/Cellar/glfw/3.4/lib \
-lglut -lGLEW -lglfw -framework OpenGL -w
```

# Potential errors and fixes

Include the following headers for macbooks.

```cpp
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
```

To fix in VS Code hit cmd + shift + p and look for C/C++: Edit configurations (JSON) and set up your JSON file like this:

```json
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "${workspaceFolder}/**",
                "/opt/homebrew/Cellar/glew/2.2.0_1/include",
                "/opt/homebrew/Cellar/freeglut/3.6.0/include",
                "/opt/homebrew/Cellar/glfw/3.4/include"
            ],
            "defines": [],
            "macFrameworkPath": [
                "/System/Library/Frameworks",
                "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks"
            ],
            "compilerPath": "/usr/bin/clang",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "macos-clang-arm64"
        }
    ],
    "version": 4
}
```

Finally run the program running the following command:

```bash
./main
```

To see just the robot model and its movement, compile the file robot.cpp using the following instruction.


```bash
g++ -o robot robot.cpp \
-I/opt/homebrew/Cellar/glew/2.2.0_1/include \
-I/opt/homebrew/Cellar/freeglut/3.6.0/include \
-I/opt/homebrew/Cellar/glfw/3.4/include \
-L/opt/homebrew/Cellar/glew/2.2.0_1/lib \
-L/opt/homebrew/Cellar/freeglut/3.6.0/lib \
-L/opt/homebrew/Cellar/glfw/3.4/lib \
-lglut -lGLEW -lglfw -framework OpenGL -w
```

Then run the program running the following command:

```bash
./robot
```

To view the robot model from different camera projection angles, press the key `V` or `v` and then click on any number key between `1` and `9`. This provided freedom to visualize the robot better during the hirarchial architecture development of the model.

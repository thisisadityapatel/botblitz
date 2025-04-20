### Description

3D minigame designed in C+ OpenGL where players are tasked with shooting waves of robots (Optimus Prime), players must hit all robots before they reach the finish line (behind the cannon). If a robot reaches the end it dies, once all robots are dead the level resets.

https://github.com/user-attachments/assets/6d1b335e-ca60-440b-9557-83b02721017d

https://github.com/user-attachments/assets/37946ad6-0d0c-4a24-9611-c754dddf8018

There are three levels to the game, at with each level robots move faster making it difficult to hunt using the defensive cannon.

Click `7` for `Level 1` - Easy (Default)  
Click `8` for `Level 2` - Medium  
Click `9` for `Level 3` - Hard

### Compiling Information

Dependency installation:

```bash
brew install glew@2.2.0_1 freeglut@3.6.0 glfw@3.4
```

Compile:

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

Include the following headers if using macOS.

```cpp
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
```

To fix in VS Code, hit `CMD + shift + p` and look for `C/C++`: Edit configurations (JSON) and set up the JSON file:

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

Finally, start the game:

```bash
./main
```

To see just the robot model and its movement, compile `robot.cpp` saperately using the following instruction.


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

Run the robot visulization program:

```bash
./robot
```

To view the robot model from different camera projection angles, press the key `V` or `v` and then click on any number key between `1` and `9`. This provided freedom to visualize the robot better during the hirarchial architecture development of the model.

## The Project
FaZoN is a framework made by me in my free time.

Features:
- 2D rendering using SFML
- ImGUI
- Input detection coming from keyboard, mouse or controller
- Action Keys allowing multiple binds to be used for the same action
- Events system

## Solution compilation and parameters
Before doing anything, some things to pay attention to:
- The solution is **Windows only**, it's the only OS I use so I don't have cross platform in mind when working on FaZoN.
- The solution is compiled in **32bits**.
- I use **Visual Studio 2022**, so there will be some parameters to change in the projects properties if you use an other version, you may have some compilation issues aswell.

## Getting started

### Directories hierarchy
This project uses a specific directory hierarchy:
```
├── FrameWork
│   ├── FrameWork
|   |   └── Code
|   |   |   ├── Externals
|   |   |   └── FZN (1)
|   |   └── Dependencies (2)
│   ├── Scripts (3)
│   ├── ProjectCreator (4)
│   └── DllLauncher (5)
|
├── Project Category (6)
│   └── Project (7)
|       ├── Code
|       |   ├── Sources
|       |   └── Project.vcxproj
|       └── Data
|
└── FaZoN.sln
```

1) The sources of the framework, include path starts here.
2) All the libs and files used by the framework
3) Batch files used in the build events of the framework and the projects, you'll need those for everything to work smoothly.
4) An application to help you create a project to work with FaZoN with all the right settings set up.
5) Some projects might need to be executed via Dll files, you'll have to use the DllLauncher in those cases.
6) Initially, there are three project categories, **Games**, **Samples** and **Utilities**, they don't really matter aside from cleaning the space a little. The scripts will however assume there is a category directory so don't put your project directly at the root of FaZoN.
7) Projects must always follow this structure, a `Code` and a `Data` folder, an Intermediate folder will add itself when the project will be compiled. In the `Code` folder, place your `vcxproj` and a folder containing your source files, the name doesn't matter.

### Project Creator
Since the solution folder hierarchy is a bit specific, I created myself a project creator, putting everything in it's rightful place for me. It also renames and prepares a bunch of variables and blocs of code/vcxproj properties.
You'll have to compile the Project Creator either in Debug or Release to create its .exe, or directly launch it from your IDE.

Once the project creator is launched, follow these steps to create a new project:
1) Click on the `Locate Solution` button and select `FaZoN.sln` in the explorer. You can check the indicated path is valid as it will be displayed on the right of the window.
2) Select the category of your project from the first combo box, this will determine in which subfolder it will go. Those categories are generated from the folders found next to `FaZoN.sln`, if you add folders in explorer, you will see them here.
3) Enter the name of the project, it will name the `vcxproj` accordingly and other variables in the code.
4) Select a project type, Game or Application. This doesn't change much, it will just create a different folder in `My Documents` depending on what you chose.
5) If you want, you can rename the source folder. There is an include path added in the vcxproj which starts at the `Code` folder, this means the includes of your project will look like this `#include "Source/MyHeader.h"`. I sometimes prefer to rename this folder so my includes are a little more clear as to which project they come from, ex: `#include "FZN/Managers/FazonCore.h"`.
6) Chose whether your project is gonna output an .exe or .dll file. This will add a command in the vcxproj properties, using the DllLauncher to launch the compiled dll file.
7) Don't worry if you see an empty box labeled `Needed libraries`, it's a WIP.
8) Hit the `Create` button.
9) You'll see logs appearing in the right text box, hopefully without errors.
10) Now, all you have to do is add your new project to the solution. Remember to put the FaZoN Framework in the new project build dependencies aswell.

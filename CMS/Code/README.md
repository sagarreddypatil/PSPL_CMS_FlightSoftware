# Code for CraterMaker Special

## Filesystem Hirearchy
```
Code
|  README.md <- this file
|--Example PlatformIO Project <- Doesn't actually exist, demonstration only
|  |  platformio.ini  <- PlatformIO Config File
|  |  .gitignore <- Tells git what files shouldn't be pushed to the repo
|  |--src <- Main code folder
|     |  main.cpp <- Main script, has setup() and loop()
|     |  example.cpp <- small auxillary script
|  |--include <- Header files for axuillary scripts in the src folder
|     |  example.h <- Header file for example.cpp
|  |--lib <- Project Specific Libraries
|  |--test <- Code for testing the code
|  |--.pio <- Contains compiler outputs
|--Libraries <- Contains common libraries shared by all/some PlatformIO projects
|  |--Example Library <- Doesn't actually exist, demonstration only
|     |--main.c/main.cpp <- Main library code file
|     |--main.h <- Header file
```

## Usage
### Working on a project
If you want to work on a project, let's use VDAQ for this example:
1. Open VSCode
2. Click on PlatformIO symbol on the left panel
3. Click on `Open`(under PIO Home)
4. Click on `Open Project`
5. Navigate to wherever this folder is, and then select VDAQ
6. Click on the `Open "VDAQ"` button


### Working on a Library
If you want to work on a library, just open the library folder in VSCode.

In order to get intellisense, you will need to copy the .vscode/c_cpp_properties.json file from one of the other projects(such as VDAQ or BangBang), doesn't really matter which.

### Adding a new PlatformIO project
Add a new PlatformIO project as usual, and make sure to select this folder as the project folder.

Add the following line to the `platformio.ini` file inside the new project
```
lib_extra_dirs = ../Libraries/
```

This will make sure that the libraries in the `Libraries` folder can be imported in this project.
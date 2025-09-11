# StarletEngine
A modular OpenGL engine written in C++

# Features
- Custom Scene Loader & Scene Saver supporting Models, Lights, Cameras
- Primitive Generation: Triangles, Squares, Cubes, SquareGrids, CubeGrids
- Lighting, Transformation handling, and multi-camera support

## Building the Project
This project uses **CMake**. Follow these steps to build:

### 1. Clone the Repository
```bash
git clone https://github.com/Masonlet/StarletEngine.git
cd LearningOpenGL
```

### 2. Create a Build Directory and Generate Build Files
```bash
mkdir build
cd build 
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```
`-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` flag generates a `compile_commands.json` file  
Can be safely omitted on Windows if you're using Visual Studio

### 3. Build the Project
- **Linux**:
  ```bash
  make
  ```

- **Windows**:
  ```bash
  cmake --build .
  ```
  Or open the generated `.sln` file in Visual Studio and build the solution.

## Runtime Controls  
| **Key**       | **Action**             |
| ------------- | ---------------------- |
| Esc           | Exit program           |


## Debug Controls
| **Key**       | **Action**             |
| ------------- | ---------------------- |
| 0–9           | Switch between cameras |
| P             | Toggle Wireframe       |
| C             | Toggle Cursor          |

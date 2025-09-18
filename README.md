# StarletEngine
A modular OpenGL engine written in C++

# Features
- Custom Scene Loader & Scene Saver supporting Models, Lights, Cameras
- Primitive Generation: Triangles, Squares, Cubes, SquareGrids, CubeGrids
- Lighting, Transformation handling, and multi-camera support

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

## Building the Project
### Using as a Dependency

```cmake
include(FetchContent)

FetchContent_Declare(StarletEngine GIT_REPOSITORY https://github.com/Masonlet/StarletEngine.git GIT_TAG main)
FetchContent_MakeAvailable(StarletEngine)

target_link_libraries(YourAppName PRIVATE StarletEngine)
```

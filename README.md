# OpenGLRenderer
A (currently lightweight) 3D renderer built from scratch in C++ and OpenGL, implementing modern real-time rendering techniques such as Bloom, SSAO, HDR, and shadow mapping. The renderer leverages the GLM math library, GLFW for windowing, OpenGL for graphics, Assimp for model loading, and stb_image for image loading.

A Physics folder exists for an in-development physics engine. 

# Features
- **Modern OpenGL Core Profile usage**
- **Real-time 3D Rendering Pipeline**
  - Depth testing and face culling
  - Multiple 3D models, textures, and materials

- **Lighting**
  - Directional, Point, and Spot lights
  - Blinn-Phong shading

- **Shadow Mapping**
  - Basic shadow maps
  - Point shadow mapping with cubemaps

- **Deferred Rendering Pipeline**
  - G-buffer creation (position, normal, albedo/specular)
  - Multiple light accumulation in a single pass

- **Post-Processing**
  - High Dynamic Range (HDR)
  - Bloom effect

- **Screen Space Ambient Occlusion (SSAO)**
  - View-space position and normal reconstruction
  - Configurable sample kernel and noise textures

## Lighting and Shadows
### Multiple Lights
<img width="1918" height="1085" alt="4 Lights" src="https://github.com/user-attachments/assets/cceda474-ac5e-4941-9639-495891f399ab" />

### Shadows and SSAO
<img width="1910" height="1078" alt="1 Light 4 Walls" src="https://github.com/user-attachments/assets/a72036d0-8c7c-480f-b250-a4ba1abcfcdf" />

### SSAO texture
<img width="1914" height="1078" alt="SSAO texture" src="https://github.com/user-attachments/assets/17897210-14c9-464c-a7e1-30fd36d94d4f" />

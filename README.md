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

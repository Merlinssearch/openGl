TODO : 
1. Trying to write an own Rendering Engine .
2. Convert it to an Own Game Engine
3. Drink Wine and smoke a cigarett on what i accomplished <3 

```markdown
# OpenGL Learning Notes

I gave an LLM photos of my Hand written Note book and it Transcribed it into an MD file. 
Yea i am one of these guys who love Hand writting <3 

## Rendering Pipeline (OpenGL)

> **Core Concept:** The pipeline is an effective way to parallelize execution across GPU core clusters.

| Pipeline Stage | Description & Operations |
| :--- | :--- |
| **Vertex Specification** | Input stream: 3D coordinates ($x, y, z$), colors, normals, etc. GPU needs to know Byte-size, Offset, Multiplier to parse data. |
| **Vertex Shader** | Transforms 3D points to 2D screen coordinates on monitor (Projection / Transformation). |
| **Tessellation** *(Optional)* | Low-poly model subdivision for added detail based on Level of Detail (LOD). |
| **Geometry Shader** *(Optional)* | Transformation of an "object" (e.g., calculate center point of a triangle and generate a pyramid). |
| **Vertex-Post Processing** | Consists of: Transform Feedback (Optional), Clipping, and Perspective Division. |
| **Primitive Assembly** | Topological assembly (e.g., `GL_TRIANGLES`, `GL_LINES`). Includes Face Culling (ignoring non-visible faces/points). |
| **Rasterization** | Determines which pixels are at the center of primitives and should be rasterized/steered. |
| **Fragment Shader** | Calculates pixel Color + Opacity ($RGBA$). |
| **Per-Sample Operations** | Discards elements outside the projection bounds (discarding what cannot be seen). |

---

## Vertex Input & OpenGL Objects

### Basic Data Layout
We take a chunk of data and send it to the GPU:

```cpp
float vertices[] = {
    x1, y1, z1,  // Point 1
    x2, y2, z2,  // Point 2
    x3, y3, z3   // Point 3
};

```

* **Geometry:** 3 Vertices, 3 Edges, 1 Face.

### Core OpenGL Objects

1. **Vertex Array Object (VAO):** Defines *how* to access and parse your VBO data. (How is data structured/parsed).
2. **Vertex Buffer Object (VBO):** Contains the actual raw data stored sequentially in memory.

```cpp
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(...); // Stores: [x1|y1|z1|x2|y2|z2|x3|y3|z3] (3 Floats per position)

glGenVertexArrays(1, &myVAO);
glBindVertexArray(myVAO);

```

---

## Shaders & Data Interop

### Vertex & Fragment Shader Lifecycle

* **Vertex Shader:** Transforms positions.
* **Fragment Shader:** Transforms colors.

```cpp
// 1. Send source code & compile
glShaderSource(...);   // Sends the source code
glCompileShader(...);  // Compiles the code

// 2. Link into Shader Program Pipeline
GLuint programID = glCreateProgram();
glAttachShader(programID, vertexShader);
glAttachShader(programID, fragmentShader);
glLinkProgram(programID);

```

### Data Transfer (CPU $\rightarrow$ GPU)

To pass simple, global data between CPU and Shader/GPU, use **Uniforms**.

* **Uniforms:** Global variables shared between linked shader programs.
* **Accessing Uniforms:** Managed via `glGetUniformLocation()` and updated via `glUniform*()` (e.g., `glUniform1f`, `glUniformMatrix4fv`).

---

## Interleaved Data Layouts & Shader Pipeline

### Combined Attribute Buffers

Interleaving position and color attributes inside a single VBO:

$$\text{1 Vertex} = 6 \text{ Floats} = 6 \times 4 \text{ Bytes} = 24 \text{ Bytes stride}$$

```
VBO Data Layout:
|  Vertex 1  |  Vertex 2  |  Vertex 3  |
| x y z | r g b | x y z | r g b | x y z | r g b |
  └───┘   └───┘
   Pos    Color

```

**VAO Configuration:**

* **Attribute 0:** Position ($x, y, z$)
* **Attribute 1:** Color ($r, g, b$)

### Passing Data Across Pipeline Stages

Data descriptions within VAO match inputs of the first shader stage, passing outputs downstream:

```
[ First Shader (Vertex) ]  ──>  out vec3 ourColor;
            │
      Linked Data Pipeline
            ▼
[ Second Shader (Fragment) ] ──>  in vec3 ourColor;
                                 out vec4 FragColor;

```

---

## 3D Rotation Matrices

### Matrix Definitions

#### X-Axis Rotation

$$\begin{pmatrix} 1 & 0 & 0 & 0 \\ 0 & \cos\theta & -\sin\theta & 0 \\ 0 & \sin\theta & \cos\theta & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix} \cdot \begin{pmatrix} x \\ y \\ z \\ 1 \end{pmatrix} = \begin{pmatrix} x \\ \cos(\theta) \cdot y - \sin(\theta) \cdot z \\ \sin(\theta) \cdot y + \cos(\theta) \cdot z \\ 1 \end{pmatrix}$$

#### Y-Axis Rotation

$$\begin{pmatrix} \cos\theta & 0 & \sin\theta & 0 \\ 0 & 1 & 0 & 0 \\ -\sin\theta & 0 & \cos\theta & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix} \cdot \begin{pmatrix} x \\ y \\ z \\ 1 \end{pmatrix} = \begin{pmatrix} \cos(\theta) \cdot x + \sin(\theta) \cdot z \\ y \\ -\sin(\theta) \cdot x + \cos(\theta) \cdot z \\ 1 \end{pmatrix}$$

#### Z-Axis Rotation

$$\begin{pmatrix} \cos\theta & -\sin\theta & 0 & 0 \\ \sin\theta & \cos\theta & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix} \cdot \begin{pmatrix} x \\ y \\ z \\ 1 \end{pmatrix} = \begin{pmatrix} \cos(\theta) \cdot x - \sin(\theta) \cdot y \\ \sin(\theta) \cdot x + \cos(\theta) \cdot y \\ z \\ 1 \end{pmatrix}$$

---

### GLSL Implementation Example

Manual Y-rotation inside a vertex shader:

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    // Manual Y-Rotation component calculation
    vec4 position = vec4(
        cos(time) * aPos.x + sin(time) * aPos.z,
        aPos.y,
        -sin(time) * aPos.x + cos(time) * aPos.z,
        1.0
    );
    
    gl_Position = position;
}

```

```

```

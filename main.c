#include "glad/glad.h"
#include <GLFW/glfw3.h> 
#include <cglm/cglm.h> 
#include <stdbool.h>
#include <stdio.h>
#include <math.h> 
#include <stdlib.h>
#include <sys/stat.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./dependencies/stb_image.h"

// note to my self STOP USING DIFFRENT CASE EITHER UPPER CASE OR LOWER SNAKE CASE BRO WHAT THE HELL !!!!!
// or use a fucking lsp 

typedef struct  {
  float *vertices;
  int attributeCounter; // 
  int *size;
  size_t bufferSize;
  unsigned int *indices; 
  size_t indicesSize;
}vertexAttributes;

// Todo: implement EBO in create mesh
typedef struct renderID {
  unsigned int VBO;
  unsigned int VAO;
  unsigned int EBO; 
}renderID;

// For Shader SourceCode
typedef struct shaderInfo {
  unsigned int type;
  const char *filePath; 
}shaderInfo;

// dont forget openGL fails sometimes silently yeaa 

void print_opengl_infos() {
  printf("==================================================================\n"); 
  printf("Debug Info \n");
  printf("==================================================================\n"); 
  printf("GPU Vendor                   : %s\n", glGetString(GL_VENDOR));
  printf("GPU Renderer                 : %s\n", glGetString(GL_RENDERER));
  printf("GL Version                   : %s\n", glGetString(GL_VERSION));
  printf("GLSL Version                 : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  GLint max_attributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attributes);
  printf("Max Vertex Attributes        : %d\n", max_attributes);

  GLint max_texture_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
  printf("Max Texture Size             : %d x %d\n", max_texture_size, max_texture_size);

  GLint max_texture_units;
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);
  printf("Max Combined Texture Units   : %d\n", max_texture_units);

  GLint max_color_attachments;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
  printf("Max FBO Color Attachments    : %d\n", max_color_attachments);
  
  GLint max_uniforms;
  glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max_uniforms);
  printf("Max Vertex Uniform Components: %d\n", max_uniforms);
  
  printf("==================================================================\n"); 
}


// yea the only input we need is to escape from here aaahhhh xD 
void processInput(GLFWwindow *window){
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

// TODO : How do i get the current height and width
// if its dynamic , like in hyprland 

GLFWwindow* init_Window() {
  if (!glfwInit()) {
      printf("Failed to initialize GLFW\n");
      return NULL;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
  
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL){
      printf("Failed to create GLFW window\n");
      glfwTerminate();
      return NULL;
  }
  glfwMakeContextCurrent(window);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
      printf("Failed to initialize GLAD\n");
      return NULL;
  } 
  
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
  glEnable(GL_DEPTH_TEST); // for 3d stuff
  return window;
} 

// dont forget to free this 

char *getFileContent (char *file) {
  struct stat st;
  // stat(file, &st);
  if (stat(file, &st) != 0) {
    printf("Fehler: stat konnte die Datei nicht lesen!\n");
    return NULL;
  }
  long size = st.st_size;  
  
  FILE *fptr;
  fptr = fopen(file, "rb");
  
  char *content = (char*)malloc(size * sizeof(char) + 1);
  if (content == NULL) {
    printf("allocation failed\n");
    return NULL;
  }

  if(fptr != NULL) {
    fread(content, sizeof(char) , size , fptr );
    content[size] = '\0';
    fclose(fptr);  
  }else {
    free(content);
    return NULL;
  }
  
  return content; 
}  

renderID createMesh(vertexAttributes attributesObject) {
  
  unsigned int VAO , VBO , EBO;
  glGenVertexArrays(1, &VAO);   
  glBindVertexArray(VAO);
  
  glGenBuffers(1, &VBO);  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);  

  glBufferData(GL_ARRAY_BUFFER, attributesObject.bufferSize , attributesObject.vertices, GL_STATIC_DRAW);
  
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, attributesObject.indicesSize, attributesObject.indices, GL_STATIC_DRAW);

  int stride = 0;
  for (int i = 0 ; i < attributesObject.attributeCounter ; i++) {
      stride += attributesObject.size[i];
  } 
  int offset = 0; 
  for (int id = 0 ; id < attributesObject.attributeCounter ; id++) {
    //////////////////////////////////////////////////////////
    // Definition : glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    glVertexAttribPointer(id,
                          attributesObject.size[id] , 
                          GL_FLOAT, 
                          GL_FALSE, 
                          stride * sizeof(float), 
                          (void*)(offset*sizeof(float))
                          );
    //////////////////////////////////////////////////////////
    glEnableVertexAttribArray(id);
    offset += attributesObject.size[id];
  } 
  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  renderID meshID;
  meshID.VAO = VAO;
  meshID.EBO = EBO;
  meshID.VBO = VBO;
  return meshID;
}

unsigned int loadShaderProgram (shaderInfo ShadersSourceCode[], int ShaderSourceCodeCount) {
  
  unsigned int shaderProgram = glCreateProgram(); 
  
  char infoLog[512];
  int success;
  for (int i = 0 ; i < ShaderSourceCodeCount ; i++ ) {
    // do i need to free ShadersSource ?
    // i think so bro , its the pointer that getFileContent gives u back and the 
    // function allocate some memory on the heap 
    const char *ShaderSource = getFileContent((char*)ShadersSourceCode[i].filePath);
    if (!ShaderSource){
      printf("loadShaderProgram failed !\n");
      return 0;
    }
    unsigned int Shader = glCreateShader(ShadersSourceCode[i].type);
    glShaderSource(Shader, 1, &ShaderSource, NULL);
    glCompileShader(Shader);
    free((char*)ShaderSource);
    ShaderSource = NULL;   

    glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(Shader, 512, NULL, infoLog);
      printf("Vertex Shader Error:\n%s\n", infoLog);
      // in openGL 0 is an invalid id 
      // we use that later to exit the program
      glDeleteShader(Shader);
      glDeleteProgram(shaderProgram);
      return 0;
    }
    glAttachShader(shaderProgram, Shader);
    glDeleteShader(Shader);  
    // ps. is this the right time to delete them ?? ?!? ?
    // because when its compiled i dont need the source code or am i dumb ? 
  } 
  
  glLinkProgram(shaderProgram);
    
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      printf("Shader Program Link Error:\n%s\n", infoLog);
      return 0;
  }
  // shit i forgot why i comment this
  // glDeleteShader(vertexShader);
  // glDeleteShader(fragmentShader);   
  return shaderProgram;
}

// TODO : A betch process for diffrent object

unsigned int loadTexture (char *imagePath) {
  /* unsigned int texture; */
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true); // Flip Y-axis so textures aren't upside down
  unsigned char *data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
  if (data){
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else{
    printf("Failed to load texture");
  }
  stbi_image_free(data);

  return texture;
}

void render(unsigned int shaderProgram ,unsigned int VAO ,  unsigned int indiciesCounter , unsigned int texture) {

  // Render Setup
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Uniform Setup
  float timeValue = glfwGetTime();
  int timeID = glGetUniformLocation(shaderProgram, "time");

  glUseProgram(shaderProgram);
  glUniform1f(timeID,timeValue);

  // texture stuff
  if (texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
  }
  glBindVertexArray(VAO);
  // TODO Render more then 1 object ....
  for (int i = 0 ; i < 3 ; i++) {
    glDrawElements(GL_TRIANGLES , indiciesCounter , GL_UNSIGNED_INT, 0);
  }
  // bro stop these fucking magic numbers what does 0 or 3 means ????
  // glDrawArrays(GL_TRIANGLES, 0 , 3 );

}


int main() {
  
  ////////////////////////////////////////////////////////////////////
  // vertices Stuff (maybe move this shit somewhere else for better reading = )   
  // maybe using inline function to init stuff ? 
  ////////////////////////////////////////////////////////////////////

  // TODO create a math function that creates EBO indices and vertices cords array
  // for cubes , triangle etc so i can create them dynamicly


  // this vertices i copyed from LLM to test it
  float vertices[] = {
    // Positions          // Colors           // Texture Coords (UV)
    // Front face (+Z)
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

    // Back face (-Z)
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 0.0f,

    // Left face (-X)
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 0.0f,

    // Right face (+X)
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,   1.0f, 0.0f,

    // Top face (+Y)
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,   1.0f, 1.0f,

    // Bottom face (-Y)
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f
  };

// 36 indices total (6 faces * 2 triangles * 3 vertices)
unsigned int indices[] = {
    0,  1,  2,      2,  3,  0,    // Front
    4,  5,  6,      6,  7,  4,    // Back
    8,  9,  10,     10, 11, 8,    // Left
    12, 13, 14,     14, 15, 12,   // Right
    16, 17, 18,     18, 19, 16,   // Top
    20, 21, 22,     22, 23, 20    // Bottom
  };

  int attributeCounter = 3 ;
  int size[] = { 3, 3 , 2};
  /* glm_rotate(); */
  vertexAttributes triangle = {
    .vertices = vertices,  
    .indices = indices,
    .attributeCounter = attributeCounter,
    .size = size,  // Attribute size 
    .bufferSize = sizeof(vertices),
    .indicesSize = sizeof(indices),
  };
  unsigned int indicesCounter =  triangle.indicesSize / sizeof(unsigned int);
  //////////////////////////////////
  // Window Stuff  
  //////////////////////////////////

  GLFWwindow *window = init_Window(); 
  if (!window) return -1;
  print_opengl_infos();
  
  //////////////////////////////////
  // loadShaderProgram
  //////////////////////////////////

  shaderInfo ShadersSourceCode[] = {
    {GL_VERTEX_SHADER, "shader/vertexshader.glsl"},
    {GL_FRAGMENT_SHADER , "shader/fragmentshaderSource.glsl"}
  };
  
  size_t ShaderSourceCodeCount = sizeof(ShadersSourceCode) / sizeof(ShadersSourceCode[0]); 
  renderID meshIDstuff = createMesh(triangle);
  unsigned int shaderProgram = loadShaderProgram(ShadersSourceCode, ShaderSourceCodeCount);
  
  if (shaderProgram == 0) {
    printf("function loadShaderProgram failed , have fun debuggin <3 \n");
    return -1 ;
  }

  //////////////////////////////////
  // load Texture
  //////////////////////////////////
  char *pathToImage = "textures/test.jpg";
  unsigned int texture = loadTexture(pathToImage);
  //////////////////////////////////
  // main loop obviously
  //////////////////////////////////
  
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    render(shaderProgram, meshIDstuff.VAO , indicesCounter, texture);
    glfwSwapBuffers(window); // fenster mgmt i dont really know what it does xD
    glfwPollEvents(); // input log que 
  }
  // cleanUP
  // maybe better in create mesh function ? 
  
  glDeleteVertexArrays(1, &(meshIDstuff.VAO));
  glDeleteBuffers(1, &(meshIDstuff.VBO));
  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0; 
}

// PS research 
// 1 .how to handle Memory leaks or perse Memory in GPU 
// glDeleteBuffers(1, &(meshIDstuff.VBO)); it seems you could delelte the VBO in create Mesh 
// because the VAO hold the infromation where the memory is 
// 2. glMultiDrawElementsIndirect for multi stuff yea 


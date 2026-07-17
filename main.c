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
}vertexAttributes;

typedef struct renderID {
  unsigned int VBO;
  unsigned int VAO;
  // unsigned int EBO;
}renderID;

typedef struct shaderInfo {
  unsigned int type;
  const char *filePath; 
}shaderInfo;

// dont forget openGL fails sometimes silently yeaa 

void print_opengl_infos() {
  printf("========================================================================\n"); 
  printf("Debug Info \n");
  printf("========================================================================\n"); 
  printf("GPU Vendor                    : %s\n", glGetString(GL_VENDOR));
  printf("GPU Renderer                  : %s\n", glGetString(GL_RENDERER));
  printf("GL Version                    : %s\n", glGetString(GL_VERSION));
  printf("GLSL Version                  : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  GLint max_attributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attributes);
  printf("Max Vertex Attributes         : %d\n", max_attributes);

  GLint max_texture_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
  printf("Max Texture Size              : %d x %d\n", max_texture_size, max_texture_size);

  GLint max_texture_units;
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);
  printf("Max Combined Texture Units    : %d\n", max_texture_units);

  GLint max_color_attachments;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
  printf("Max FBO Color Attachments     : %d\n", max_color_attachments);
  
  GLint max_uniforms;
  glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max_uniforms);
  printf("Max Vertex Uniform Components : %d\n", max_uniforms);
  
  printf("========================================================================\n"); 
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
// for debug and re rendering stuff on the window 
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
  
  return window;
} 

// dont forget to free this 
// also this code seams a little bit sloppy 
// fix later for now it works
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
  
  unsigned int VAO , VBO;
  glGenVertexArrays(1, &VAO);   
  glBindVertexArray(VAO);
  
  glGenBuffers(1, &VBO);  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);  

  glBufferData(GL_ARRAY_BUFFER, attributesObject.bufferSize , attributesObject.vertices, GL_STATIC_DRAW);
  
  int stride = 0;
  for (int i = 0 ; i < attributesObject.attributeCounter ; i++) {
      stride += attributesObject.size[i];
  } 
  int offset = 0; 
  for (int id = 0 ; id < attributesObject.attributeCounter ; id++) {
    // glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    glVertexAttribPointer(id,
                          attributesObject.size[id] , 
                          GL_FLOAT, 
                          GL_FALSE, 
                          stride * sizeof(float), 
                          (void*)(offset*sizeof(float)));
    glEnableVertexAttribArray(id);
    offset += attributesObject.size[id];
  } 
  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  renderID meshID;  
  meshID.VAO = VAO;
  // meshID.EBO = EBO;
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
  // glDeleteShader(vertexShader);
  // glDeleteShader(fragmentShader);   
  return shaderProgram;
}


void render(unsigned int shaderProgram ,unsigned int VAO ) {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  float timeValue = glfwGetTime();
  int vertexColorLocation = glGetUniformLocation(shaderProgram, "time");

  glUseProgram(shaderProgram);
  // glUniform4f for GLOBAL variables between shader programms
  glUniform1f(vertexColorLocation,timeValue);
  
  glBindVertexArray(VAO);
  // bro stop these fucking magic numbers what does 0 or 3 means ???? 
  glDrawArrays(GL_TRIANGLES, 0 , 3 );

}

// renderID loadTexture (char *filePath) {
//
//
// } 

int main() {
  
  ////////////////////////////////////////////////////////////////////
  // vertices Stuff (maybe move this shit somewhere else for better reading = )   
  ////////////////////////////////////////////////////////////////////
  
  float vertices[] = {
    // positions 3float  // colors 3 floats
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
  };
  int attributeCounter = 2 ; 
  int size[] = {3,3};
  vertexAttributes triangle = {
    .vertices = vertices,  
    .attributeCounter = attributeCounter,
    .size = size,
    .bufferSize = sizeof(vertices),
  };
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
  // main loop obviously
  //////////////////////////////////
  
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    render(shaderProgram, meshIDstuff.VAO);
    glfwSwapBuffers(window); // fenster mgmt
    glfwPollEvents(); // input log que 
  }
  
  // cleanUP
  // in create mesh function ? 
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


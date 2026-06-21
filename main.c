#include "glad/glad.h"
#include <GLFW/glfw3.h> 
#include <cglm/cglm.h> 
#include <stdbool.h>
#include <stdio.h>
#include <math.h> 
#include <stdlib.h>
#include <sys/stat.h>

typedef struct renderID {
  unsigned int VBO;
  unsigned int VAO;
  // unsigned int EBO;
}renderID;


// TODO implment some debuggin printf function

void print_opengl_infos() {
  printf("GPU Vendor  : %s\n", glGetString(GL_VENDOR));
  printf("GPU Renderer: %s\n", glGetString(GL_RENDERER));
  printf("GL Version  : %s\n", glGetString(GL_VERSION));
  printf("GLSL Version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  GLint max_attributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attributes);
  printf("Max Vertex Attributes:           %d\n", max_attributes);

  GLint max_texture_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
  printf("Max Texture Size:                %d x %d\n", max_texture_size, max_texture_size);

  GLint max_texture_units;
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);
  printf("Max Combined Texture Units:      %d\n", max_texture_units);

  GLint max_color_attachments;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
  printf("Max FBO Color Attachments:       %d\n", max_color_attachments);
  
  GLint max_uniforms;
  glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max_uniforms);
  printf("Max Vertex Uniform Components:   %d\n", max_uniforms);
}


void processInput(GLFWwindow *window){
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}


// TODO : How do i get the current height and width
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

renderID createMesh() {
  // Descripton of the Data 
  // how does the GPU parse the Data 
  // where is Postion     , Color       ,etc 
  //          Attribute 1 , Attribute 2 
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);   
  glBindVertexArray(VAO);
 
  float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
  };  
  // unsigned int indices[] = {
  //     0, 1, 3, 
  //     1, 2, 3   
  // };
  // actual Data 
  unsigned int VBO;
  glGenBuffers(1, &VBO);  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);  
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  // unsigned int EBO;
  // glGenBuffers(1, &EBO);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  

  // Descripton of the Data , so the GPU can parse it 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0); 
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  renderID meshID;  
  meshID.VAO = VAO;
  // meshID.EBO = EBO;
  meshID.VBO = VBO;
  return meshID;
}

unsigned int loadShaderProgram (char *path , char *path2) {
  const char *vertexShaderSource = getFileContent(path);
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  free((char*)vertexShaderSource);
  vertexShaderSource = NULL;   

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      printf("Vertex Shader Error:\n%s\n", infoLog);
      // in openGL 0 is an Invalid id 
      // we use that later to exit the program
      return 0;
  } 

  const char *fragmentShaderSource = getFileContent(path2); 
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  free((char*)fragmentShaderSource);
  fragmentShaderSource = NULL; 
  
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      printf("Fragment Shader Error:\n%s\n", infoLog);
      return 0;
  }

  // SHADER PROGRAMM LINKEN
  unsigned int shaderProgram = glCreateProgram(); 
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
    
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      printf("Shader Program Link Error:\n%s\n", infoLog);
      return 0;
  }
  
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);   
  return shaderProgram;
} 

void render(unsigned int shaderProgram ,unsigned int VAO ) {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  float timeValue = glfwGetTime();
  float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
  float redValue = (cos(timeValue) / 2.0f) + 0.5f;
  float blueValue = (tan(timeValue) / 2.0f) + 0.5f;
  int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
  
  glUseProgram(shaderProgram);
  glBindVertexArray(VAO);
  glUniform4f(vertexColorLocation, redValue, greenValue, 0.0f, 1.0f);
  // thats we use EBO'S 
  // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glDrawArrays(GL_TRIANGLES, 0 , 3 );

}

int main() {
  GLFWwindow *window = init_Window(); 
  if (!window) return -1;
  print_opengl_infos();
  // init stuff
  // unsigned int VAO = createMesh();
  renderID meshIDstuff = createMesh();
  unsigned int shaderProgram = loadShaderProgram("vertexshader.glsl" , "fragmentShaderSource.glsl"); 
  
  if (shaderProgram == 0) {
    printf("function loadShaderProgram failed , have fun debuggin <3 \n");
    return -1 ;
  } 
  
  // main loop obviously
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    render(shaderProgram, meshIDstuff.VAO);
    glfwSwapBuffers(window); // fenster mgmt
    glfwPollEvents(); // input log que 
  }
  
  // cleanUP
  glDeleteVertexArrays(1, &(meshIDstuff.VAO));
  glDeleteBuffers(1, &(meshIDstuff.VBO));
  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0; 
}

// PS research how to handle Memory leaks or perse Memory in GPU 
// glDeleteBuffers(1, &(meshIDstuff.VBO)); it seems you could delelte the VBO in create Mesh 
// because the VAO hold the infromation where the memory is 

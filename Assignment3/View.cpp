#include "View.h"
#include "PolygonMesh.h"
#include "sgraph/ScenegraphInfo.h"
#include "sgraph/SceneXMLReader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "FileReader.h"
using namespace std;

View::View()
    : staticCamera(nullptr), revolveCamera(nullptr)
{   
  WINDOW_WIDTH = WINDOW_HEIGHT = 0;
  trackballRadius = 300;
  trackballTransform = glm::mat4(1.0);
  proj = glm::mat4(1.0);
  scenegraph = NULL;
}

View::~View()
{
  if (scenegraph!=NULL)
    delete scenegraph;

  for (auto obj: gameObjects)
  {
    if (obj != nullptr)
       delete obj;
  }
}

void View::initScenegraph(util::OpenGLFunctions &gl, const string& filename) throw(runtime_error)
{
  if (scenegraph!=NULL)
    delete scenegraph;

  program.enable(gl);
  sgraph::ScenegraphInfo<VertexAttrib> sinfo;
  sinfo = sgraph::SceneXMLReader::importScenegraph<VertexAttrib>(filename);
  scenegraph = sinfo.scenegraph;

  renderer.setContext(&gl);
  map<string,string> shaderVarsToVertexAttribs;
  shaderVarsToVertexAttribs["vPosition"] = "position";
  shaderVarsToVertexAttribs["vNormal"] = "normal";
  shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";
  renderer.initShaderProgram(program,shaderVarsToVertexAttribs);
  scenegraph->setRenderer<VertexAttrib>(&renderer,sinfo.meshes);
  program.disable(gl);

}

void View::initScenegraph(util::OpenGLFunctions &gl) throw(runtime_error)
{
  if (scenegraph!=NULL)
    delete scenegraph;

  program.enable(gl);
  sgraph::ScenegraphInfo<VertexAttrib> sinfo;
  sinfo = sgraph::SceneXMLReader::importScenegraph<VertexAttrib>(sceneGraphPath);
  scenegraph = sinfo.scenegraph;

  renderer.setContext(&gl);
  map<string,string> shaderVarsToVertexAttribs;
  shaderVarsToVertexAttribs["vPosition"] = "position";
  shaderVarsToVertexAttribs["vNormal"] = "normal";
  shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";
  renderer.initShaderProgram(program,shaderVarsToVertexAttribs);
  scenegraph->setRenderer<VertexAttrib>(&renderer,sinfo.meshes);
  program.disable(gl);

}


std::vector<std::string> View::ParseConfig(const std::string& _path)
{
    std::string content = FileReader::ReadFile(_path);
    std::vector <std::string> tokens;
    std::stringstream check1(content);
    std::string intermediate;

    while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }
    return tokens;
}

void View::init(util::OpenGLFunctions& gl) throw(runtime_error)
{
  //do this if your initialization throws an error (e.g. shader not found,
  //some model not found, etc.
  //  throw runtime_error("Some error happened!");

  //create the shader program
  program.createProgram(gl,
                        string("shaders/default.vert"),
                        string("shaders/default.frag"));

  //assuming it got created, get all the shader variables that it uses
  //so we can initialize them at some point
  shaderLocations = program.getAllShaderVariables(gl);


  std::string configFileName;
  printf("config file path: ");
  std::cin >> configFileName;
  std::vector<std::string> tokens = ParseConfig(configFileName);
  sceneGraphPath = tokens[CONFIG_SCENE_GRAPH_PATH_SLOT];
  glm::vec3 camInitPos(atof(tokens[CONFIG_CAMERA_INIT_POS_SLOT + 0].c_str()),
                       atof(tokens[CONFIG_CAMERA_INIT_POS_SLOT + 1].c_str()),
                       atof(tokens[CONFIG_CAMERA_INIT_POS_SLOT + 2].c_str()));
  staticCamera = new Camera();
  staticCamera->GetTransform()->SetPosition(camInitPos);

  revolveCamera = new RevolveCamera(glm::vec3(0.0f, 1.0f, 0.0f), 2.0f, 80.0f);
  revolveCamera->GetTransform()->SetPosition(camInitPos);

  gameObjects.push_back(staticCamera);
  gameObjects.push_back(revolveCamera);

  revolveCamera->SetToMainCamera();
}

void View::draw(util::OpenGLFunctions& gl)
{
  gl.glClearColor(0,0,0,1);
  gl.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl.glEnable(GL_DEPTH_TEST);

  if (scenegraph==NULL)
    return;

  program.enable(gl);

  while (!modelview.empty())
    modelview.pop();

  /*
         *In order to change the shape of this triangle, we can either move the vertex positions above, or "transform" them
         * We use a modelview matrix to store the transformations to be applied to our triangle.
         * Right now this matrix is identity, which means "no transformations"
         */
  modelview.push(glm::mat4(1.0));
//  modelview.top() = modelview.top() *
//      glm::lookAt(glm::vec3(0.0f,50.0f,80.0f),
//                  glm::vec3(0.0f,50.0f,0.0f),
//                  glm::vec3(0.0f,1.0f,0.0f)) *
//      trackballTransform;

  modelview.top() = modelview.top() * Camera::s_MainCamera->GetViewMat();


  /*
        *Supply the shader with all the matrices it expects.
        */
  gl.glUniformMatrix4fv(shaderLocations.getLocation("projection"),
                        1,
                        false,
                        glm::value_ptr(proj));

  //gl.glPolygonMode(GL.GL_FRONT_AND_BACK,GL3.GL_LINE); //OUTLINES

  scenegraph->draw(modelview);
  gl.glFlush();

  program.disable(gl);
}

void View::Update()
{
    for (auto gobj: gameObjects)
    {
        gobj->Update();
    }
}

void View::mousePressed(int x,int y)
{
  mousePos = glm::vec2(x,y);
}

void View::mouseReleased(int x,int y)
{

}

void View::mouseDragged(int x,int y)
{
  glm::vec2 newM = glm::vec2((float)x,(float)y);

  glm::vec2 delta = glm::vec2((float)(newM.x-mousePos.x),(float)(newM.y-mousePos.y));
  mousePos = newM;

  trackballTransform =
      glm::rotate(glm::mat4(1.0),delta.x/trackballRadius,glm::vec3(0.0f,1.0f,0.0f)) *
      glm::rotate(glm::mat4(1.0),delta.y/trackballRadius,glm::vec3(1.0f,0.0f,0.0f)) *
      trackballTransform;
}

void View::keyReleased(int key)
{
    switch (key) {
    case Qt::Key_T:
        Camera::s_MainCamera = revolveCamera;
        break;
    case Qt::Key_G:
        Camera::s_MainCamera = staticCamera;
        break;
    default:
        break;
    }

}

void View::reshape(util::OpenGLFunctions& gl,int width,int height)
{
  //record the new width and height
  WINDOW_WIDTH = width;
  WINDOW_HEIGHT = height;

  /*
     * The viewport is the portion of the screen window where the drawing
     * would be placed. We want it to take up the entire area of the window
     * so we set the viewport to be the entire window.
     * Look at documentation of glViewport
     */

  gl.glViewport(0, 0, width, height);

  /*
     * This sets up the part of our virtual world that will be visible in
     * the screen window. Since this program is drawing 2D, the virtual world
     * is 2D. Thus this window can be specified in terms of a rectangle
     * Look at the documentation of glOrtho2D, which glm::ortho implements
     */

  proj = glm::perspective(glm::radians(120.0f),(float)width/height,0.1f,10000.0f);

}

void View::dispose(util::OpenGLFunctions& gl)
{
  //clean up the OpenGL resources used by the object
  scenegraph->dispose();
  renderer.dispose();
  //release the shader resources
  program.releaseShaders(gl);
}

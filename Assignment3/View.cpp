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

#include "XMLMeshrenderer.h"
#include "FileReader.h"

using namespace std;

View::View()
    : ctrl(nullptr), switcher(nullptr), staticCamera(nullptr), keyCtrlCamera(nullptr)
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
  lights = sinfo.lights;

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
  lights = sinfo.lights;
  scenegraph->setLights(lights);

  renderer.setContext(&gl);
  map<string,string> shaderVarsToVertexAttribs;
  shaderVarsToVertexAttribs["vPosition"] = "position";
  shaderVarsToVertexAttribs["vNormal"] = "normal";
  shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";
  renderer.initShaderProgram(program,shaderVarsToVertexAttribs);
  scenegraph->setRenderer<VertexAttrib>(&renderer,sinfo.meshes);
  program.disable(gl);

}

void View::initMeshRendererForGameObjs(util::OpenGLFunctions &gl) throw (runtime_error)
{
    XMLMeshRenderer* xmr = new XMLMeshRenderer(staticCamera, cameraModelPath, &renderer);
    xmr->SetMeshOffset(glm::vec3(0.0f, 10.0f, -30.0f));
    staticCamera->AddComponent(static_cast<Component*>(xmr), XMLMeshRenderer::ComponentID);

////    XMLMeshRenderer* xmr2 = new XMLMeshRenderer(keyCtrlCamera, cameraModelPath, &renderer);
////    xmr2->SetMeshOffset(glm::vec3(0.0f, 10.0f, -30.0f));
////    keyCtrlCamera->AddComponent((Component*)xmr2, XMLMeshRenderer::ComponentID);

    XMLMeshRenderer* xmr2 = new XMLMeshRenderer(drone, controllerModelPath, &renderer);
    xmr2->SetMeshOffset(glm::vec3(0.0f, -12.0f, -50.0f));
    drone->AddComponent((Component*)xmr2, XMLMeshRenderer::ComponentID);

    XMLMeshRenderer* xmr3 = new XMLMeshRenderer(dp1, propellerModelPath, &renderer);
    xmr3->SetMeshOffset(glm::vec3(0.0f, 0.0f, 0.0f));
    dp1->AddComponent((Component*)xmr3, XMLMeshRenderer::ComponentID);

    XMLMeshRenderer* xmr4 = new XMLMeshRenderer(dp2, propellerModelPath, &renderer);
    xmr4->SetMeshOffset(glm::vec3(0.0f, 0.0f, 0.0f));
    dp2->AddComponent((Component*)xmr4, XMLMeshRenderer::ComponentID);

    XMLMeshRenderer* xmr5 = new XMLMeshRenderer(drone_light, droneLightModelPath, &renderer);
    xmr5->SetMeshOffset(glm::vec3(0.0f, 0.0f, 0.0f));
    drone_light->AddComponent((Component*)xmr5, XMLMeshRenderer::ComponentID);

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
                        string("shaders/phong-multiple.vert"),
                        string("shaders/phong-multiple.frag"));

  //assuming it got created, get all the shader variables that it uses
  //so we can initialize them at some point
  shaderLocations = program.getAllShaderVariables(gl);


  std::string configFileName;
  printf("config file path: ");
  std::cin >> configFileName;
  std::vector<std::string> tokens = ParseConfig(configFileName);
  sceneGraphPath = tokens[CONFIG_SCENE_GRAPH_PATH_SLOT];
  cameraModelPath = tokens[CONFIG_CAMERA_MODEL_SLOT];
  glm::vec3 camInitPos(atof(tokens[CONFIG_CAMERA_INIT_POS_SLOT + 0].c_str()),
                       atof(tokens[CONFIG_CAMERA_INIT_POS_SLOT + 1].c_str()),
                       atof(tokens[CONFIG_CAMERA_INIT_POS_SLOT + 2].c_str()));
  controllerModelPath = tokens[CONFIG_CONTROLLER_MODEL_SLOT];
  propellerModelPath = tokens[CONFIG_PROPELLER_MODEL_SLOT];
  droneLightModelPath = tokens[CONFIG_LIGHT_STRUCTURE_MODEL_SLOT];

//  sceneGraphPath = tokens[0];
//  glm::vec3 camInitPos(atof(tokens[1 + 0].c_str()),
//                       atof(tokens[1 + 1].c_str()),
//                       atof(tokens[1 + 2].c_str()));



  ctrl = new Controller();
  ctrl->SetToGlobalController();

//  drone = new Drone(ctrl);
//  drone->GetTransform()->SetDirection(glm::vec3(0.0f, 0.0f, -1.0f));
//  dp1 = new DronePropeller();
//  dp2 = new DronePropeller();
//  drone->AddChild(dp1);
//  drone->AddChild(dp2);
//  dp1->GetTransform()->Translate(glm::vec3(20.0f, 0.0f, 0.0f) + glm::vec3(0.0f, -12.0f, 50.0f));
//  dp2->GetTransform()->Translate(glm::vec3(-20.0f, 0.0f, 0.0f) + glm::vec3(0.0f, -12.0f, 50.0f));
//  drone_light = new DroneLight();
//  drone->AddChild(drone_light);
//  drone_light->GetTransform()->Translate(glm::vec3(0.0f, 5.0f, 0.0f) + glm::vec3(0.0f, -8.0f, 10.0f));
//  drone_light->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
//  drone->GetTransform()->SetPosition(camInitPos);


  staticCamera = new Camera();
  staticCamera->GetTransform()->SetPosition(camInitPos);

  keyCtrlCamera = new KeyCtrlCamera(ctrl);
  keyCtrlCamera->GetTransform()->SetPosition(camInitPos);

  switcher = new CameraSwitchcer(keyCtrlCamera, staticCamera);
  rayTracerSwitcher = new RayTracerSwitcher();

  gameObjects.push_back(staticCamera);
  gameObjects.push_back(keyCtrlCamera);
//  gameObjects.push_back(drone);
//  gameObjects.push_back(dp1);
//  gameObjects.push_back(dp2);
//  gameObjects.push_back(drone_light);

  keyCtrlCamera->SetToMainCamera();
}

void View::draw(util::OpenGLFunctions& gl)
{
  gl.glClearColor(0,0.0f,0.0f,1);
  gl.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl.glEnable(GL_DEPTH_TEST);

  if (scenegraph==NULL)
    return;

  program.enable(gl);

  ///////////////////////////////
  ///// DRAW FIRST VIEWPORT /////
  ///////////////////////////////
  while (!modelview.empty())
    modelview.pop();

  modelview.push(glm::mat4(1.0));
  modelview.top() = modelview.top() * switcher->GetCam1()->GetViewMat();
  /*
   *Supply the shader with all the matrices it expects.
   */
  gl.glUniformMatrix4fv(shaderLocations.getLocation("projection"),
                        1,
                        false,
                        glm::value_ptr(switcher->GetCam1()->GetProjMat()));
  // apply lights
  ApplyLights(gl);
  scenegraph->updateBoundingBox(modelview);
  scenegraph->draw(modelview);
  for (auto obj: gameObjects)
  {
      obj->Render(switcher->GetCam1());
  }

  gl.glFlush();

  ////////////////////////////////
  ///// DRAW SECOND VIEWPORT /////
  ////////////////////////////////
//  while (!modelview.empty())
//    modelview.pop();
//  modelview.push(glm::mat4(1.0));
//  modelview.top() = modelview.top() * switcher->GetCam2()->GetViewMat();

//  gl.glUniformMatrix4fv(shaderLocations.getLocation("projection"),
//                        1,
//                        false,
//                        glm::value_ptr(switcher->GetCam2()->GetProjMat()));

//  gl.glScissor(WINDOW_WIDTH * 0.7f, WINDOW_WIDTH * 0.7f, WINDOW_WIDTH * 0.3f, WINDOW_HEIGHT * 0.3f);
//  gl.glEnable(GL_SCISSOR_TEST);
//  gl.glViewport(WINDOW_WIDTH * 0.7f, WINDOW_WIDTH * 0.7f, WINDOW_WIDTH * 0.3f, WINDOW_HEIGHT * 0.3f);
//  gl.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//  ApplyLights(gl);
//  scenegraph->updateBoundingBox(modelview);
//  scenegraph->draw(modelview);
//  for (auto obj: gameObjects)
//  {
//      obj->Render(switcher->GetCam2());
//  }
//  gl.glDisable(GL_SCISSOR_TEST);
//  gl.glFlush();

  if (rayTracerSwitcher->IsRayTracerEnabled())
  {
      raytrace(2000, 2000, modelview);
      rayTracerSwitcher->Reset();
  }

  ////////////////////////////////
  /////     RESET VIEWPORT   /////
  ////////////////////////////////
  gl.glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  program.disable(gl);
}

void View::Update()
{
    switcher->Update();
    rayTracerSwitcher->Update();
    for (auto gobj: gameObjects)
    {
        gobj->Update();
    }
}

void View::ApplyLights(util::OpenGLFunctions& gl) const
{
    struct _lightLocation
    {
        int ambient = -1;
        int diffuse = -1;
        int specular = -1;
        int position = -1;
        int spotdirection = -1;
    };

    std::vector<_lightLocation> lls;
    for (int i=0; i<lights.size(); ++i)
    {
        std::stringstream ss;
        ss << "light[" << i << "]";
        std::string sVarName = ss.str();
        _lightLocation ll;
        ll.ambient = shaderLocations.getLocation(sVarName + ".ambient");
        ll.diffuse = shaderLocations.getLocation(sVarName + ".diffuse");
        ll.specular = shaderLocations.getLocation(sVarName + ".specular");
        ll.position = shaderLocations.getLocation(sVarName + ".position");
        ll.spotdirection = shaderLocations.getLocation(sVarName + ".spotdirection");
        lls.push_back(ll);
    }
        /*Light properties are now being sent to the shader*/
    gl.glUniform1i(shaderLocations.getLocation("numLights"), lights.size());

    for (int i=0; i<lights.size(); ++i)
    {

        gl.glUniform3fv(lls[i].ambient, 1, glm::value_ptr(lights[i].getAmbient()));
        gl.glUniform3fv(lls[i].specular, 1, glm::value_ptr(lights[i].getSpecular()));
        gl.glUniform3fv(lls[i].diffuse, 1, glm::value_ptr(lights[i].getDiffuse()));
        gl.glUniform4fv(lls[i].position, 1, glm::value_ptr(glm::vec4(modelview.top() * lights[i].getPosition())));
        gl.glUniform4fv(lls[i].spotdirection, 1, glm::value_ptr(glm::vec4(modelview.top() * lights[i].getSpotDirection())));
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

void View::keyPressed(int key)
{
    ctrl->KeysCallback(ctrl->QtKeyCodeTranslate(key), KEY_PRESSED);
}

void View::keyReleased(int key)
{
    ctrl->KeysCallback(ctrl->QtKeyCodeTranslate(key), KEY_RELEASED);
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

void View::raytrace(int width, int height, stack<glm::mat4> &modelView){

    std::cout << "[raytrace] start\n";
    glm::vec4 startPoint = glm::vec4(0, 0, 0, 1);// in the view coordinate system
    float nearDistance = 0.1;//the distance of near plane with the startpoint
    float farDistance = 10000.0;// the distance of far plane with the startpoint
    float angle = 120.0f;
    float windowHeight = nearDistance * tan(glm::radians(120.0f));
    float ratio = width / height;
    float windowWidth = ratio * windowHeight;
    float widthRatio = windowWidth / (width / 2);
    float heightRatio = windowHeight / (height / 2);
    QImage image = QImage(width, height, QImage::Format_RGB32);
    QColor color;
    Ray ray;// the ray object
    for(int widthIndex = 0; widthIndex < width; widthIndex++){
        for(int heightIndex = 0; heightIndex < height; heightIndex++){
            float directionX = (widthIndex - (width / 2)) * widthRatio;
            float directionY = ((height / 2) - heightIndex) * heightRatio;
            float directionZ = - nearDistance;
            glm::vec4 direction = glm::normalize(glm::vec4(directionX, directionY, directionZ, 0.0f));// in the view coordinate system
            ray.setStartPoint(startPoint);
            ray.setDirection(direction);
            color = scenegraph->raycast(ray, modelView, 0);
//            cout << "Pixel Color :\nred : " << color.redF() << " green: " << color.greenF() << " blue : " << color.blueF() << endl;
            image.setPixelColor((width - 1 - widthIndex), (height - 1 -heightIndex), color);
            //delete ray;
        }
    }
    image.save("image.png");
    std::cout << "[raytrace] complete.\n";
    //delete image;

}

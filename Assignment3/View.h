#ifndef VIEW_H
#define VIEW_H

#include "OpenGLFunctions.h"
#include <exception>
using namespace std;
#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/GLScenegraphRenderer.h"
#include "ShaderLocationsVault.h"
#include "ObjectInstance.h"
#include "VertexAttrib.h"
#include <stack>

#include "Camera.h"
#include "CameraSwitchcer.h"
#include "RevolveCamera.h"
#include "Drone.h"
#include "DronePropeller.h"
#include "DroneLight.h"
#include "KeyCtrlCamera.h"
#include "raytracerswitcher.h"

#define CONFIG_SCENE_GRAPH_PATH_SLOT 0
#define CONFIG_CONTROLLER_MODEL_SLOT 1
#define CONFIG_PROPELLER_MODEL_SLOT 2
#define CONFIG_LIGHT_STRUCTURE_MODEL_SLOT 3
#define CONFIG_CAMERA_MODEL_SLOT 4
#define CONFIG_CAMERA_INIT_POS_SLOT 5

using namespace std;

/*
 * This class encapsulates all our program-specific details. This makes our
 * design better if we wish to port it to another C++-based windowing
 * library
 */

class View
{
public:
    View();
    ~View();
    /*
     * This is called when the application is being initialized. We should
     * do all our initializations here. This is also the first function where
     * OpenGL commands will work (i.e. don't do any OpenGL related stuff in the
     * constructor!)
     */
    void init(util::OpenGLFunctions& e) throw(runtime_error);

    void initScenegraph(util::OpenGLFunctions& e,const string& in) throw(runtime_error);
    void initScenegraph(util::OpenGLFunctions &gl) throw(runtime_error);
    void initMeshRendererForGameObjs(util::OpenGLFunctions &gl) throw (runtime_error);
    /*
     * This function is called whenever the window is to be redrawn
     */
    void draw(util::OpenGLFunctions& e);

    /*
     * This function is called whenever the window is reshaped
     */
    void reshape(util::OpenGLFunctions& gl,int width,int height);

    /*
     * This function is called whenever the window is being destroyed
     */
    void dispose(util::OpenGLFunctions& gl);

    void mousePressed(int x,int y);
    void mouseReleased(int x,int y);
    void mouseDragged(int x,int y);

    void keyReleased(int key);
    void keyPressed(int key);

    void Update();

    void ApplyLights(util::OpenGLFunctions& gl) const;

    void raytrace(int width, int height, stack<glm::mat4> &modelView);

    std::vector<std::string> ParseConfig(const std::string& _path);

private:
    //record the current window width and height
    int WINDOW_WIDTH,WINDOW_HEIGHT;
    //the projection matrix
    glm::mat4 proj;
    //the trackball transform
    glm::mat4 trackballTransform;
    //the radius of the virtual trackball
    float trackballRadius;
    //the mouse position
    glm::vec2 mousePos;
    //the modelview matrix
    stack<glm::mat4> modelview;
    //the scene graph
    sgraph::Scenegraph *scenegraph;
    //the list of shader variables and their locations within the shader program
    util::ShaderLocationsVault shaderLocations;
    //the GLSL shader
    util::ShaderProgram program;
    sgraph::GLScenegraphRenderer renderer;

    // -------------------------------------------------------------------

    // basic stuff
    Controller* ctrl;
    CameraSwitchcer* switcher;
    Camera* staticCamera;
    KeyCtrlCamera* keyCtrlCamera;
    RayTracerSwitcher* rayTracerSwitcher;

    // scene stuff
    std::vector<GameObject*> gameObjects;
    Drone *drone;
    DroneLight *drone_light;
    DronePropeller *dp1, *dp2;

    std::vector<util::Light> lights;


    // input config stuff
    std::string sceneGraphPath;
    std::string cameraModelPath;
    std::string controllerModelPath;
    std::string propellerModelPath;
    std::string droneLightModelPath;
};

#endif // VIEW_H

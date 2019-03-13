QT += core
QT += core
QT += gui widgets xml

CONFIG += c++11

TARGET = Scenegraphs
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += ../headers

TEMPLATE = app

SOURCES += main.cpp \
    OpenGLWindow.cpp \
    View.cpp \
    Camera.cpp \
    CameraSwitchcer.cpp \
    Component.cpp \
    Controller.cpp \
    FileReader.cpp \
    GameObject.cpp \
    KeyCtrlCamera.cpp \
    Transform.cpp \
    XMLMeshrenderer.cpp \
    Drone.cpp \
    DroneLight.cpp \
    DronePropeller.cpp \
    MLight.cpp

HEADERS += \
    OpenGLWindow.h \
    VertexAttrib.h \
    View.h \
    sgraph/AbstractNode.h \
    sgraph/GLScenegraphRenderer.h \
    sgraph/GroupNode.h \
    sgraph/INode.h \
    sgraph/IScenegraph.h \
    sgraph/LeafNode.h \
    sgraph/Scenegraph.h \
    sgraph/scenegraphinfo.h \
    sgraph/SceneXMLReader.h \
    sgraph/TransformNode.h \
    Camera.h \
    CameraSwitchcer.h \
    Component.h \
    Controller.h \
    FileReader.h \
    GameObject.h \
    KeyCtrlCamera.h \
    Transform.h \
    XMLMeshrenderer.h \
    Drone.h \
    DroneLight.h \
    DronePropeller.h \
    DroneWithCamera.h

DISTFILES += \
    shaders/default.frag \
    shaders/default.vert


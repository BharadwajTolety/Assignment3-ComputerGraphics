#ifndef XMLMESHRENDERER_H
#define XMLMESHRENDERER_H

#include "Component.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/GLScenegraphRenderer.h"
#include "VertexAttrib.h"

class Camera;
class XMLMeshRenderer: public Component
{
public:
    static int ComponentID;
public:
    XMLMeshRenderer(GameObject* _obj, const std::string& _xml_path, sgraph::GLScenegraphRenderer* _renderer);
    ~XMLMeshRenderer();
    void Init(const std::string& _xml_path, sgraph::GLScenegraphRenderer* _renderer);
    void Update();
    void Render(Camera* _cam);
public:
    inline void SetMeshOffset(const glm::vec3& _offset) { m_MeshOffset = _offset; }

public:
    glm::mat4 GetModelTransformGlobal() const;

private:
    sgraph::Scenegraph* m_Model;
    glm::vec3 m_MeshOffset;
};

#endif // XMLMESHRENDERER_H

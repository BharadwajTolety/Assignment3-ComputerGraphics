#include "XMLMeshrenderer.h"
#include "sgraph/ScenegraphInfo.h"
#include "sgraph/SceneXMLReader.h"
#include "PolygonMesh.h"
#include "Camera.h"
#include <stack>

int XMLMeshRenderer::ComponentID = 0;

XMLMeshRenderer::XMLMeshRenderer(GameObject* _obj, const std::string& _xml_path, sgraph::GLScenegraphRenderer* _renderer)
    : Component(_obj, ComponentID), m_Model(nullptr), m_MeshOffset(0.0f, 0.0f,0.0f)
{
    Init(_xml_path, _renderer);
}

XMLMeshRenderer::~XMLMeshRenderer()
{
    if (m_Model)
    {
        m_Model->dispose();
        delete m_Model;
    }
}

void XMLMeshRenderer::Init(const std::string& _xml_path, sgraph::GLScenegraphRenderer* _renderer)
{
    sgraph::ScenegraphInfo<VertexAttrib> sinfo;
    sinfo = sgraph::SceneXMLReader::importScenegraph<VertexAttrib>(_xml_path);
    m_Model = sinfo.scenegraph;
    m_Model->setRenderer<VertexAttrib>(_renderer,sinfo.meshes);
}

void XMLMeshRenderer::Init(const std::string &_xml_path, sgraph::GLScenegraphRenderer *_renderer, std::vector<util::Light>& _lightsStorage)
{
    sgraph::ScenegraphInfo<VertexAttrib> sinfo;
    sinfo = sgraph::SceneXMLReader::importScenegraph<VertexAttrib>(_xml_path);
    m_Model = sinfo.scenegraph;
    m_Model->setRenderer<VertexAttrib>(_renderer,sinfo.meshes);
    for (auto l: sinfo.lights)
    {
        _lightsStorage.push_back(l);
    }
}

void XMLMeshRenderer::Update()
{

}

glm::mat4 XMLMeshRenderer::GetModelTransformGlobal() const
{
    GameObject* host = GetHost();
    glm::mat4 model = host->GetTransform()->GetModelTransformMat();

    GameObject* _host_ptr = host;
    while (_host_ptr->GetParent() != nullptr)
    {
        auto parent = _host_ptr->GetParent();
        model = parent->GetTransform()->GetModelTransformMat() * model;
        _host_ptr = _host_ptr->GetParent();
    }
    return model;
}

void XMLMeshRenderer::Render(Camera* _cam)
{
    glm::mat4 view = _cam->GetViewMat();

    GetHost()->GetTransform()->Translate(m_MeshOffset);
    glm::mat4 model = GetModelTransformGlobal();
    GetHost()->GetTransform()->Translate(-m_MeshOffset);

    std::stack<glm::mat4> stk;
    stk.push(view * model);
    m_Model->draw(stk);
}

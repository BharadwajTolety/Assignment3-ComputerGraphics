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

void XMLMeshRenderer::Update()
{

}

void XMLMeshRenderer::Render(Camera* _cam)
{
    glm::mat4 view = _cam->GetViewMat();
    GetHost()->GetTransform()->Translate(m_MeshOffset);
    glm::mat4 model = GetHost()->GetTransform()->GetModelTransformMat();
    GetHost()->GetTransform()->Translate(-m_MeshOffset);
    std::stack<glm::mat4> stk;
    stk.push(view * model);
    m_Model->draw(stk);
}

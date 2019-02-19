#include "GameObject.h"

GameObject::GameObject(const char* _tag)
    : m_Tag(_tag), m_Parent(nullptr), m_Transform(nullptr)
{
    m_Transform = new Transform();
    for (unsigned int i = 0; i<MAX_COMPONENT_NUM; ++i)
    {
        m_Components[i] = nullptr;
    }
}

void GameObject::Update()
{

}

void GameObject::AddComponent(Component* _c, const unsigned int& _id)
{
    m_Components[_id] = _c;
}

void GameObject::AddChild(GameObject *_obj)
{
    m_Childs.push_back(_obj);
}

GameObject::~GameObject()
{
    for (auto child: m_Childs)
    {
        if (child != nullptr)
            delete child;
    }
}


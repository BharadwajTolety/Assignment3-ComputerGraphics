#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* _obj, const int& _id)
    : m_Host(_obj), id(_id)
{}

Component::~Component()
{}

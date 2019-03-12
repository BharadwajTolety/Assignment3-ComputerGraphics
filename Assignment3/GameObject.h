#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Component.h"
#include "Transform.h"
#include <vector>
#define MAX_COMPONENT_NUM 10

class Camera;

/**
    Every object within the scene should be derived from GameObject
**/
class GameObject
{
public:
    GameObject(const char* _name = "");
    virtual ~GameObject();
public:
    virtual void Update();
    virtual void Render(Camera* _cam);

public:
    inline Transform* GetTransform() const { return m_Transform; }
    inline Component** GetComponents() { return m_Components; }
    inline const char* GetName() const {return m_Name;}
    inline const char* GetTag() const {return m_Tag;}
    inline void SetParent(GameObject* _p) { m_Parent = _p; }
    inline GameObject* GetParent() const { return m_Parent; }
    inline std::vector<GameObject*> GetChilds() const { return m_Childs; }

public:
    void AddComponent(Component* _c, const unsigned int& _id);
    void AddChild(GameObject* _obj);
    void RenderCF(Camera* _cam); // render with children first

protected:
    const char* m_Name;
    const char* m_Tag;

    GameObject* m_Parent;
    std::vector<GameObject*> m_Childs;

    Component* m_Components[MAX_COMPONENT_NUM];

    Transform* m_Transform;
};

#endif // GAMEOBJECT_H

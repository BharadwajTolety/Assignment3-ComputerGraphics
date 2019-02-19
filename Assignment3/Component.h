#ifndef COMPONENT_H
#define COMPONENT_H

class GameObject;

class Component
{
public:
    Component(GameObject* _obj, const int& _id);
    virtual ~Component();

public:
    inline GameObject* GetHost() const { return m_Host; }
    inline void SetComponentID(const int& _id);
    inline bool GetIsActive() const { return m_IsActive;}
    inline void SetIsActive(const bool& _ia) { m_IsActive = _ia;}
protected:
    GameObject* m_Host;
    bool m_IsActive;
    int id;
};

#endif // COMPONENT_H

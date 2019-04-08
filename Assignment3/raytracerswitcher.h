#ifndef RAYTRACERSWITCHER_H
#define RAYTRACERSWITCHER_H


class RayTracerSwitcher
{
public:
    RayTracerSwitcher();
    void Update();
public:
    inline bool IsRayTracerEnabled() const { return m_RayTracerEnabled; }
    inline void Reset() { m_RayTracerEnabled = false; }
private:
    bool m_RayTracerEnabled;
};

#endif // RAYTRACERSWITCHER_H

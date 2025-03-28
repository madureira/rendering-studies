#pragma once

class App
{
public:
    virtual ~App() = default;
    virtual void Update() = 0;
};

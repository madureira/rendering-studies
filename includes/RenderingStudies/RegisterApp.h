#pragma once

#include "AppRegistry.h"
#include <memory>
#include <utility>

template <class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#define REGISTER_APP(ClassType)                                    \
    namespace                                                      \
    {                                                              \
    struct ClassType##Registrator                                  \
    {                                                              \
        ClassType##Registrator()                                   \
        {                                                          \
            AppRegistry::Instance().Register(                      \
                #ClassType,                                        \
                [](Window* w) -> std::unique_ptr<App> {            \
                    return make_unique<ClassType>(w);              \
                });                                                \
        }                                                          \
    };                                                             \
    static ClassType##Registrator global_##ClassType##Registrator; \
    }

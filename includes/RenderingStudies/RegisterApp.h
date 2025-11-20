#pragma once

#include "AppRegistry.h"

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
                    return std::make_unique<ClassType>(w);         \
                });                                                \
        }                                                          \
    };                                                             \
    static ClassType##Registrator global_##ClassType##Registrator; \
    }

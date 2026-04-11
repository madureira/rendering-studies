#pragma once

#include "DemoRegistry.h"

#define REGISTER_DEMO(ClassType, webCompatible)                          \
    namespace                                                            \
    {                                                                    \
    struct ClassType##Registrator                                        \
    {                                                                    \
        ClassType##Registrator()                                         \
        {                                                                \
            DemoRegistry::Instance().Register(                           \
                #ClassType,                                              \
                [](Window& w, Camera& c) -> Demo* {                      \
                    return new ClassType(w, c);                          \
                },                                                       \
                (webCompatible));                                        \
        }                                                                \
    };                                                                   \
    static ClassType##Registrator global_##ClassType##Registrator;       \
    }

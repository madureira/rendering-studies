#pragma once

#include "AppRegistry.h"

#define REGISTER_APP(ClassType, webCompatible)                           \
    namespace                                                            \
    {                                                                    \
    struct ClassType##Registrator                                        \
    {                                                                    \
        ClassType##Registrator()                                         \
        {                                                                \
            AppRegistry::Instance().Register(                            \
                #ClassType,                                              \
                [](Window& w, Camera& c) -> App* {                       \
                    return new ClassType(w, c);                          \
                },                                                       \
                (webCompatible));                                        \
        }                                                                \
    };                                                                   \
    static ClassType##Registrator global_##ClassType##Registrator;       \
    }

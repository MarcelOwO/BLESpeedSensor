//
// Created by marce on 6/12/2025.
//

#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <memory>

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
void CHECK(T condition, std::string msg)
{
    static_assert(std::is_same<T, bool>::value, "Condition must be bool");
    if (!condition)
    {
        Serial.println(msg.c_str());
        std::exit(-1);
    }
}
#endif //TEMPLATES_H

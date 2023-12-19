#pragma once

#include <iostream>
#include <vector>
#include "Message.h"

class TopicObserver {
public:
    virtual ~TopicObserver() = default;
    virtual void notify(const Message& message) = 0;
};
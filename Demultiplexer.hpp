#pragma once

#include "EventHandler.hpp"
#include <map>

class Demultiplexer
{
  protected:
    int m_numHandlers;

  public:
    Demultiplexer();
    virtual ~Demultiplexer();

    virtual int registerEvent(handle_t handle, eventType et) = 0;
    virtual int unregisterEvent(handle_t handle, eventType et) = 0;
    virtual int waitEvents(std::map<handle_t, EventHandler *> &handlers) = 0;
    virtual void setnumHandlers(int nHandlers);
};
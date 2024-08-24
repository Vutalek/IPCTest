#ifndef APPLICATION_H
#define APPLICATION_H

class Application
{
public:
    virtual ~Application() {}

    virtual void init() = 0;
    virtual bool done() = 0;
    virtual void action() = 0;
    virtual void exit() = 0;
};

#endif

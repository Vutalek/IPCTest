#ifndef APPLICATION_H
#define APPLICATION_H

//Strategy pattern

class Application
{
public:
    virtual ~Application() {}

    //Application initializing
    virtual void init() = 0;

    //Check if application is done and may be closed
    virtual bool done() = 0;

    //Main activity of application
    virtual void action() = 0;

    //Exit actions
    virtual void exit() = 0;
};

#endif

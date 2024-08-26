#ifndef APPLICATIONRUNNER_H
#define APPLICATIONRUNNER_H

#include "Application.cpp"

//Strategy pattern

class ApplicationRunner
{
public:
    ApplicationRunner(
        Application* app
    )
    : application(app)
    {
    }

    ~ApplicationRunner()
    {
        delete application;
    }

    //Basic sequence of application actions
    void run()
    {
        application->init();
        while(!application->done())
            application->action();
        application->exit();
    }
private:
    Application* application;
};

#endif

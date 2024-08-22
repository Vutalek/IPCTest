#ifndef APPLICATIONRUNNER_H
#define APPLICATIONRUNNER_H

#include "Application.cpp"

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

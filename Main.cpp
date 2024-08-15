#include "ApplicationRunner.cpp"
#include "IPCTest.cpp"

#include <string>

int main(int argc, char* argv[])
{
    if (argc == 1)
        cout << "Specify program." << endl;
    else if (argc == 2)
        cout << "Specify method." << endl;
    else if (argc == 3)
        cout << "Specify file." << endl;
    else
    {
        ApplicationRunner apprun = ApplicationRunner(new IPCTest(argv[1], argv[2], argv[3]));
        apprun.run();
    }
    return 0;
}

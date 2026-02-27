#include <voxelgl/App.h>


int main()
{
    voxelgl::App app;

    if (app.init()) 
    {
        app.run();
        app.exit();
    
        return 0;
    }
    else
    {
        return -1;
    }
}
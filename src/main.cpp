#include "app/app.h"

int main()
{
    App* app = app_create();

    if (app_init(app)) 
    {
        app_run(app);
        app_destroy(app);

        return 0;
    }
    else
    {
        return -1;
    }
}
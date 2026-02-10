#include <voxelgl/App.h>

int main() {
    voxelgl::App app;

    if (!app.start()) {
        return -1;
    }

    app.run();
    app.exit();

    return 0;
}
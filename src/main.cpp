#include <voxelgl/App.h>

int main() {
    voxelgl::App app;

    if (!app.initialize()) {
        return -1;
    }

    app.run();
    app.shutdown();

    return 0;
}
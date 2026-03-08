#include "interface/interface.h"

Interface* interface_create()
{
    Interface* interface = (Interface*)malloc(sizeof (*interface));

    return interface;
}

void interface_init(Interface* interface)
{
    interface->version = 1;

    InterfaceImage block_selection_image;

    block_selection_image.position[0] = 12.0f;
    block_selection_image.position[1] = 12.0f;

    block_selection_image.size[0] = 50.0f;
    block_selection_image.size[1] = 50.0f;

    interface->interface_image_capacity = 10;

    interface->interface_image_array =
        (InterfaceImage*)malloc(
            sizeof(InterfaceImage) * interface->interface_image_capacity
        );

    interface->interface_image_array[0] = block_selection_image;
    interface->interface_image_count = 1;
}
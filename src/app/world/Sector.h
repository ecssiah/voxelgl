#pragma once

#include <cstdint>
#include "app/world/grid.h"
#include "cell.h"

struct Sector
{
    uint32_t m_version = 0;
    SectorIndex m_sector_index = 0;
    
    Cell m_cell_array[get_sector_volume_in_cells()];

    void init(SectorIndex sector_index);
};
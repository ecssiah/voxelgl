#pragma once

#include <cstdint>

#include "Cell.h"
#include "metrics.h"

struct Sector
{
    void init(SectorIndex sector_index);
    
    uint32_t m_version = 0;
    SectorIndex m_sector_index = 0;

    Cell m_cell_array[SECTOR_VOLUME_IN_CELLS];
};
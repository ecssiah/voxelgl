#include "app/world/cell.h"
#include "app/world/grid.h"

#include <cglm/cglm.h>

void Cell::init(CellIndex cell_index, SectorIndex sector_index)
{
    m_cell_index = cell_index;
    m_sector_index = sector_index;
    m_cell_face_mask = 0;
    m_block_kind = BLOCK_KIND_NONE;
}
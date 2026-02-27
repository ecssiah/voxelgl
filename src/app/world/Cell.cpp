#include "Cell.h"

#include <cglm/cglm.h>

#include "metrics.h"

void Cell::init(CellIndex cell_index)
{
    m_cell_index = cell_index;

    ivec3 cell_coordinate;
    cell_index_to_cell_coordinate(cell_index, cell_coordinate);

    m_block_type = (BlockType)(rand() % BlockType::Count);
}
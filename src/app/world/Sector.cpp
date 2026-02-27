#include "sector.h"

void Sector::init(SectorIndex sector_index)
{
    m_sector_index = sector_index;

    SectorCoordinate sector_coordinate;
    sector_index_to_sector_coordinate(sector_index, sector_coordinate);

    for (CellIndex cell_index = 0; cell_index < SECTOR_VOLUME_IN_CELLS; cell_index++)
    {
        m_cell_array[cell_index].init(cell_index);
    }

    m_version = 1;
}
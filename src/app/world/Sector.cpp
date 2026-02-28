#include "sector.h"
#include "app/world/grid.h"

void Sector::init(SectorIndex sector_index)
{
    m_sector_index = sector_index;

    for (CellIndex cell_index = 0; cell_index < SECTOR_VOLUME_IN_CELLS; cell_index++)
    {
        CellCoordinate cell_coordinate;
        cell_index_to_cell_coordinate(cell_index, cell_coordinate);

        printf("  Cell: %d (%d %d %d)\n", cell_index, cell_coordinate[0], cell_coordinate[1], cell_coordinate[2]);

        m_cell_array[cell_index].init(cell_index, sector_index);
    }

    m_version = 1;
}
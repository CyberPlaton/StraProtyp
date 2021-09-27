#include "MaptileRegions.h"

MaptileRegionID MaptileRegionStorage::g_maptileRegionID = 1;
MaptileRegionStorage* MaptileRegionStorage::g_MaptileRegionStorage = nullptr;

MaptileRegion::MaptileRegion()
{
	id = MaptileRegionStorage::getNextMaptileRegionId();
	MaptileRegionStorage::get()->addMaptileRegion(this);
}
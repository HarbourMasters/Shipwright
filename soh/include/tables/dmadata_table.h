/**
 * Select dmadata table for version
 */
#ifdef NON_MATCHING
// For non-matching builds, dmadata is generated from the specfile segments
#include "dmadata_table_spec.h"
#else
#include "tables/dmadata_table_mqdbg.h"
#endif

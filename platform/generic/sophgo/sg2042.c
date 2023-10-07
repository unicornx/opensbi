/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Authors:
 *   Inochi Amaoto <inochiama@outlook.com>
 *
 */

#include <platform_override.h>
#include <thead/c9xx_errata.h>
#include <thead/c9xx_pmu.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/timer/aclint_mtimer.h>

#define SOPHGO_SG2042_TIMER_BASE	0x70ac000000UL
#define SOPHGO_SG2042_TIMER_SIZE	0x10000UL


static int sophgo_sg2042_early_init(bool cold_boot,
				    const struct fdt_match *match)
{
	const struct sbi_platform *plat = sbi_platform_thishart_ptr();

	thead_register_tlb_flush_trap_handler();

	/*
	 * Sophgo sg2042 soc use seperate 16 timers while initiating,
	 * merge them as a single domain to avoid wasting.
	 */
	if (cold_boot)
		return sbi_domain_root_add_memrange(SOPHGO_SG2042_TIMER_BASE,
						SOPHGO_SG2042_TIMER_SIZE *
						sbi_platform_hart_count(plat),
						MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));


	return 0;
}

static u32 sophgo_sg2042_tlb_num_entries(const struct fdt_match *match)
{
	unsigned long num_tlb_entries = (unsigned long)match->data;
	return num_tlb_entries;
}

static int sophgo_sg2042_extensions_init(const struct fdt_match *match,
					 struct sbi_hart_features *hfeatures)
{
	thead_c9xx_register_pmu_device();
	return 0;
}

static const struct fdt_match sophgo_sg2042_match[] = {
	{ .compatible = "sophgo,sg2042", .data = (void*)64 },
	{ },
};

const struct platform_override sophgo_sg2042 = {
	.match_table		= sophgo_sg2042_match,
	.tlb_num_entries	= sophgo_sg2042_tlb_num_entries,
	.early_init		= sophgo_sg2042_early_init,
	.extensions_init	= sophgo_sg2042_extensions_init,
};

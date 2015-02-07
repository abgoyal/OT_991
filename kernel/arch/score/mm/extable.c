

#include <linux/module.h>

int fixup_exception(struct pt_regs *regs)
{
	const struct exception_table_entry *fixup;

	fixup = search_exception_tables(regs->cp0_epc);
	if (fixup) {
		regs->cp0_epc = fixup->fixup;
		return 1;
	}
	return 0;
}

#include <stdint.h>

void llvm_va_start(uint16_t ap_offset, uint16_t ap_selector, uint16_t last_arg_offset, uint16_t last_arg_selector)
{
	uint16_t first_arg_offset = last_arg_offset + 4;
	uint16_t carry = (first_arg_offset < last_arg_offset) ? 1 : 0;
	uint16_t first_arg_selector = last_arg_selector + carry;
	
	*(uint16_t *)(ap_offset) = first_arg_offset;
	*(uint16_t *)(ap_offset + 2) = first_arg_selector;
}

void llvm_va_end(uint16_t ap_offset, uint16_t ap_selector)
{
}

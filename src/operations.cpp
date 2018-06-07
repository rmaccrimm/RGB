#include "operations.h"
#include "processor.h"

void op::LD_imm(Processor *proc, r8 &reg)
{
    reg.set(proc->fetch_byte());
}

#include "Mapper170.h"

Mapper170::Mapper170(NES* parent) : Mapper(parent),reg() {}
//////////////////////////////////////////////////////////////////////////
// Mapper170                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper170::Reset()
{
	reg = 0;
	switch (PROM_16K_SIZE) {
	default:
	case	1:	// 16K only
		SetPROM_16K_Bank(4, 0);
		SetPROM_16K_Bank(6, 0);
		break;
	case	2:	// 32K
		SetPROM_32K_Bank(0);
		break;
	}


}

bool Mapper170::ReadAddress(WORD addr)
{
	return false;// addr == 0x7001 || addr == 0x7777;
}

bool Mapper170::WriteAddress(WORD addr)
{
	return false;// addr == 0x6502 || addr == 0x7000;
}

void Mapper170::Write(WORD addr, BYTE data)
{
	switch (addr) {
	case 0x6502:
	case 0x7000:
		reg = (data << 1) & 0x80;
		break;
	}
}

BYTE Mapper170::Read(WORD addr, BYTE& data)
{
	switch (addr) {
	case 0x7001:
	case 0x7777:
		data = reg;
		break;
	}
	return data;
}


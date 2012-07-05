#ifndef __LAUNCHPAD_H__
#define __LAUNCHPAD_H__
namespace LP {
	enum colours {
		OFF          =   0b000100,
		RED_LOW      =   0b000101,
		RED_MEDIUM   =   0b000110,
		RED_FULL     =   0b000111,
		GREEN_LOW    =   0b010100,
		AMBER_LOW    =   0b010101,
		AMBER_MEDIUM =   0b010110,
		AMBER_1      =   0b010111,
		GREEN_MEDIUM =   0b100100,
		AMBER_2      =   0b100101,
		AMBER_3      =   0b100110,
		AMBER_HIGH   =   0b100111,
		GREEN_FULL   =   0b110100,
		AMBER_4      =   0b110101,
		YELLOW       =   0b110110,
		AMBER_FULL   =   0b110111
	};

	enum modifiers {
		BLINK = 0b110011,
		RESET = 0xB00000,
	};

	const uint32_t coordToMidi[9][9] = { 
		{0xb06800,  0xb06900,  0xb06a00,  0xb06b00,  0xb06c00,  0xb06d00,  0xb06e00,  0xb06f00,  0},
		{0x900000,  0x900100,  0x900200,  0x900300,  0x900400,  0x900500,  0x900600,  0x900700,  0x900800},
		{0x901000,  0x901100,  0x901200,  0x901300,  0x901400,  0x901500,  0x901600,  0x901700,  0x901800},
		{0x902000,  0x902100,  0x902200,  0x902300,  0x902400,  0x902500,  0x902600,  0x902700,  0x902800},
		{0x903000,  0x903100,  0x903200,  0x903300,  0x903400,  0x903500,  0x903600,  0x903700,  0x903800},
		{0x904000,  0x904100,  0x904200,  0x904300,  0x904400,  0x904500,  0x904600,  0x904700,  0x904800},
		{0x905000,  0x905100,  0x905200,  0x905300,  0x905400,  0x905500,  0x905600,  0x905700,  0x905800},
		{0x906000,  0x906100,  0x906200,  0x906300,  0x906400,  0x906500,  0x906600,  0x906700,  0x906800},
		{0x907000,  0x907100,  0x907200,  0x907300,  0x907400,  0x907500,  0x907600,  0x907700,  0x907800}
	};
}




#endif

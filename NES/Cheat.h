#ifndef	__CEHAT_INCLUDED__
#define	__CEHAT_INCLUDED__

#include "typedef.h"

#include <string>
using namespace std;

// #### either/or.
#define	CHEAT_ENABLE		(1<<0)
#define	CHEAT_KEYDISABLE	(1<<1)

// #### Types of Writing
#define	CHEAT_TYPE_ALWAYS	0	// #### constantly writing
#define	CHEAT_TYPE_ONCE		1	// #### 1 time only write-in
#define	CHEAT_TYPE_GREATER	2	// #### A time larger than data.
#define	CHEAT_TYPE_LESS		3	// #### At a smaller scale.

// #### data length
#define	CHEAT_LENGTH_1BYTE	0
#define	CHEAT_LENGTH_2BYTE	1
#define	CHEAT_LENGTH_3BYTE	2
#define	CHEAT_LENGTH_4BYTE	3

class	CHEATCODE {
public:
	BYTE	enable;
	BYTE	type;
	BYTE	length;
	WORD	address;
	DWORD	data;

	string	comment;
};

class	GENIECODE {
public:
	WORD	address;
	BYTE	data;
	BYTE	cmp;
};

#endif	// !__CEHAT_INCLUDED__


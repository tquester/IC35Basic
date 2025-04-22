
#include <ic35sdk.h>
#include "data.h"
#include "compiler.h"



char __lit01[6] = "PRINT";
char __lit02[4] = "DIM";
char __lit03[3] = "AS";
char __lit04[3] = "IF";
char __lit05[5] = "THEN";
char __lit06[5] = "ELSE";
char __lit07[4] = "END";
char __lit08[8] = "INTEGER";
char __lit09[5] = "LONG";
char __lit10[6] = "SHORT";
char __lit11[7] = "STRING";
char __lit12[8] = "FLOAT";
char __lit13[4] = "FOR";
char __lit14[3] = "TO";
char __lit15[5] = "NEXT";
char __lit16[5] = "GOTO";
char __lit17[6] = "GOSUB";


BasicToken gBasToken[18] =  {
	{  NULL, OP_PRINT		},
	{  NULL, OP_DIM			},
	{  NULL, OP_AS			},
	{  NULL, OP_IF			},
	{  NULL, OP_THEN			},
	{  NULL, OP_ELSE			},
	{  NULL, OP_END			},
	{  NULL, OP_INTEGER		},
	{  NULL, OP_LONG			},
	{  NULL, OP_SHORT		},
	{  NULL, OP_STRING		},
	{  NULL, OP_FLOAT		},
	{  NULL, OP_FOR			},
	{  NULL, OP_TO           },
	{  NULL, OP_NEXT         },
	{  NULL, OP_GOTO		    },
	{  NULL, OP_GOSUB        },
	{  NULL,   0        }
};
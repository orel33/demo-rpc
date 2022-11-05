/* add program (new RPC style) */

program ADDPROG {
	version ADDVER {
		int add(int, int) = 1;
	} = 1;
} = 0x20000199;

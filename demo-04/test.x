/* TEST program (new RPC style) */

program TESTPROG {
	version TESTVER {
		double pi(void) = 1;
		int inc(int) = 2;
		int add(int, int) = 3;
		string echo(string) = 4;
	} = 1;
} = 0x20000001;

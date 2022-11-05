#include <stdlib.h>
#include "add.h"

void addprog_1(char *host)
{
	CLIENT *clnt;
	int *result_1;
	int add_1_arg1 = 10;
	int add_1_arg2 = 20;

	clnt = clnt_create(host, ADDPROG, ADDVER, "tcp");
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}

	result_1 = add_1(add_1_arg1, add_1_arg2, clnt);
	if (result_1 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}

	printf("result: %d + %d = %d\n", add_1_arg1, add_1_arg2, *result_1);

	clnt_destroy(clnt);
}

int main(int argc, char *argv[])
{
	char *host;

	if (argc < 2)
	{
		printf("usage: %s server_host\n", argv[0]);
		exit(1);
	}
	host = argv[1];
	addprog_1(host);


	return EXIT_SUCCESS;

}

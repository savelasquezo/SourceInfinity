#include <stdio.h>

void WriteVersion()
{
#ifndef __WIN32__
	FILE* fp = fopen("BESTProduction.txt", "w");

	if (fp)
	{
		fprintf(fp, "YAKUP POLAT - BEST Production Copyright © 2013-2023 Tum Haklari Saklidir.\n");
		//fprintf(fp, "%s@%s:%s\n", __USER__, __HOSTNAME__, __PWD__);
		fclose(fp);
	}
#endif
}



#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "../varnam.h"

int main(int argc, char *argv[])
{
	varnam *handle;
	int rc;
	char *msg;
	char word[200];

	FILE *input_file;
	input_file = fopen(argv[1], "r");

	if(!input_file)
		perror("ERROR : invalid input file");

	rc = varnam_init_from_lang("ml", &handle, &msg);

	if(rc != VARNAM_SUCCESS)
		perror("Could not initialize varnam");

	while(!feof(input_file))
	{
		fscanf(input_file, "%s", word);
		varnam_learn(handle, word);
	}

	fclose(input_file);
	varnam_destroy(handle);
	return 0;
}
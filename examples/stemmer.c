#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "../varnam.h"


int has_punctuation(char *word)
{
	if(word == NULL)
		return 0;

	int pos = strlen(word)-1;

	if(word[pos] == '.')
		return 1;
	else if(word[pos] == ',')
		return 1;
	else if(word[pos] == '!')
		return 1;
	else if(word[pos] == '?')
		return 1;
	else 
		return 0;
}

int main(int argc, char *argv[])
{
	varnam *handle;
	char *msg, word[200];
	int rc;
	strbuf *syllable;
	char *stemmed;

	FILE *stemfile;
	stemfile = fopen(argv[1], "r");
	syllable = strbuf_init(20);

	if(!stemfile)
	{
		printf("Input file not specified\n");
		return 1;
	}

	strbuf *string;
	string = strbuf_init(200);

	rc = varnam_init_from_lang("ml", &handle, &msg);
	
	if(rc != VARNAM_SUCCESS)
	{
		printf("Error initializing varnam\n");
		return 1;
	}

	while(!feof(stemfile))
	{
		fscanf(stemfile, "%s", word);

		if(has_punctuation(word))
			word[strlen(word)-1] = '\0';
		/*If we don't use a strbuf, varnam_stem would give a segmentation fault*/
		strbuf_add(string, word);
		printf("%s : ", word);
		vst_get_last_syllable(handle, string, syllable);

		stemmed = (char*)malloc(string->length * sizeof(char));
		printf("last : %s", strbuf_to_s(syllable));
		varnam_stem(handle, (char*)strbuf_to_s(string), false, stemmed);
		printf("%s\n", stemmed);
		string->length = strlen(strbuf_to_s(string));
		strbuf_clear(string);	
		strbuf_clear(syllable);
	}

	free(stemmed);
}
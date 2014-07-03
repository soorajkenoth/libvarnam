/*
 * Copyright (C) Navaneeth.K.N
 *
 * This is part of libvarnam. See LICENSE.txt for the license
 */

#include <check.h>
#include <stdio.h>
#include <malloc.h>
#include "testcases.h"
#include "../varnam.h"

void setup_test_data()
{
	int rc;
	char **msg;
	char *filename = get_unique_filename();

	reinitialize_varnam_instance(filename);
	ensure_schema_exists(varnam_instance, msg);
}

START_TEST (insert_stemrule)
{
	int rc;
	char *empty_word="";

	rc = varnam_create_stemrule(varnam_instance, empty_word, "ല", 1);
	assert_error(rc);

	rc = varnam_create_stemrule(varnam_instance, "ക", "ല", 0);
	assert_error(rc);

	rc = varnam_create_stemrule(varnam_instance, "ക", "ല", 4);
	assert_error(rc);

	rc = varnam_create_stemrule(varnam_instance, "ക", "ല", 1);
	assert_success(rc);
}
END_TEST

START_TEST (stem_word)
{
	int rc;
	char *word;;
	char *empty_word=NULL;

	word = (char*)malloc(sizeof("പലക"));
	strcpy(word, "പലക");
	
	rc = varnam_create_stemrule(varnam_instance, "ക", "ല", 1);
	assert_success(rc);

	rc = varnam_stem(varnam_instance, word, false);
	assert_success(rc);
	ck_assert_str_eq(word, "പലല");

	rc = varnam_stem(varnam_instance, empty_word, false);
	assert_error(rc);
}
END_TEST

TCase* get_stemmer_tests()
{
 	TCase* tcase = tcase_create("stemmer");
 	tcase_add_checked_fixture(tcase, setup, teardown);
 	tcase_add_checked_fixture(tcase, setup_test_data, NULL);
 	tcase_add_test(tcase, insert_stemrule);
 	tcase_add_test(tcase, stem_word);
 	return tcase;
}
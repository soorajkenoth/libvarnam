/*
 * Copyright (C) Navaneeth.K.N
 *
 * This is part of libvarnam. See LICENSE.txt for the license
 */

#include <check.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "testcases.h"
#include "../varnam.h"

void setup_test_data()
{
	char **msg=NULL;
	char *filename = get_unique_filename();

	reinitialize_varnam_instance(filename);
	ensure_schema_exists(varnam_instance, msg);
}

int insert_to_vst(int type,
					const char *pattern,
					const char *value1,
					const char *value2,
					const char *value3,
					const char *tag,
					int match_type,
					int priority,
					int accept_condition,
					int flags)
{
	int rc;
	sqlite3 *db=NULL;
	sqlite3_stmt *stmt;
	const char *sql = "insert into symbols (type, pattern, value1, value2, value3, tag, match_type, priority, accept_condition, flags) values (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10)";


	db = varnam_instance->internal->db;

	if(db == NULL)
		return 1;

	rc = sqlite3_prepare_v2( db, sql, -1, &stmt, NULL); 

	if(rc != SQLITE_OK)
    {
        set_last_error (varnam_instance, "Failed to add metadata : %s", sqlite3_errmsg(db));
        sqlite3_finalize( stmt );
        ck_abort_msg("Could not prepare sqlite3 statement");
    }

    sqlite3_bind_int(stmt, 1, type);
    sqlite3_bind_text(stmt, 2, pattern, -1, NULL);
    sqlite3_bind_text(stmt, 3, value1, -1, NULL);
    sqlite3_bind_text(stmt, 4, value2, -1, NULL);
    sqlite3_bind_text(stmt, 5, value3, -1, NULL);
    sqlite3_bind_text(stmt, 5, tag, -1, NULL);
    sqlite3_bind_int(stmt, 6, match_type);
    sqlite3_bind_int(stmt, 7, priority);
    sqlite3_bind_int(stmt, 8, accept_condition);
    sqlite3_bind_int(stmt, 9, flags);

    rc = sqlite3_step(stmt);

    if(rc != SQLITE_DONE)
    {
    	printf("Something is wrong\n");
    	return 1;
    }

    sqlite3_finalize(stmt);

    return 0;
}

START_TEST (insert_stemrule)
{
	int rc;
	const char *empty_word="";

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

START_TEST(syllables)
{
	int rc;   
	rc = insert_to_vst(9, "~", "", "", "", "", 1, 0, 0, 0);
	assert_success(rc);
	rc = insert_to_vst(1, "aa", "ാ", "", "", "", 1, 0, 0, 0);
	assert_success(rc);
	rc = insert_to_vst(1, "e", "എ", "െ", "", "", 1, 0, 0, 0);
	assert_success(rc);
	rc = insert_to_vst(1, "i", "ഇ", "ി", "", "", 1, 0, 0, 1);
	assert_success(rc);
	rc = insert_to_vst(4, "vi", "വി", "", "", "", 1, 0, 0, 1);
	assert_success(rc);
	rc = insert_to_vst(2, "Ta", "ട", "", "", "", 1, 0, 0, 3);
	assert_success(rc);
	rc = insert_to_vst(4, "Te", "ടെ", "", "", "", 1, 0, 0, 1);
	assert_success(rc);
	rc = insert_to_vst(2, "nna", "ന്ന", "", "", "", 1, 0, 0, 3);
	assert_success(rc);
	rc = insert_to_vst(4, "nnaa", "ന്നാ", "", "", "", 2, 0, 0, 1);
	assert_success(rc);
	rc = insert_to_vst(3, "nn", "ന്ന്", "", "", "", 1, 0, 0, 3);
	assert_success(rc);
	rc = insert_to_vst(3, "n~", "ന്", "്", "", "", 1, 0, 0, 3);
	assert_success(rc);
	rc = insert_to_vst(2, "na", "ന", "", "", "", 1, 0, 0, 3);
	assert_success(rc);

	strbuf *string = strbuf_init(strlen("ഇവിടെ") * sizeof(char));
	char *syllable = strbuf_init(strlen("ഇവിടെ") * sizeof(char));
	strbuf_add(string, "ഇവിടെ");

	rc = vst_get_last_syllable(varnam_instance, string, syllable);
	assert_success(rc);
	ck_assert_str_eq(strbuf_to_s(syllable), "ടെ");
	int count = vst_syllables_count(varnam_instance, string);
	ck_assert_int_eq(count, 3);


	strbuf_clear(string);
	strbuf_clear(syllable);

	strbuf *string2 = strbuf_init(strlen("എന്ന്") * sizeof(char));
	strbuf_add(string2, "എന്ന്");
	rc = vst_get_last_syllable(varnam_instance, string2, syllable);
	ck_assert_str_eq(strbuf_to_s(syllable), "ന്");
	count = vst_syllables_count(varnam_instance, string2);
	ck_assert_int_eq(count, 2);
}
END_TEST

START_TEST (stem_word)
{
	int rc;
	char *word=NULL, *stemmed=NULL;
	char *empty_word=NULL;

	/*level 1*/
	word = (char*)malloc(strlen("അവളെ") * sizeof(char));
	if(word == NULL)
		ck_abort_msg("Cannot allocate memory");
	
	stemmed = (char*)malloc(strlen("അവളെ") * sizeof(char));
	if(stemmed == NULL)
		ck_abort_msg("Cannot allocate memory");
	
	strcpy(word, "അവളെ");

	rc = varnam_create_stemrule(varnam_instance, "ളെ", "ൾ", 1);
	assert_success(rc);

	rc = varnam_stem(varnam_instance, word, false, stemmed);
	assert_success(rc);
	ck_assert_str_eq(stemmed, "അവൾ");
	free(word);
	free(stemmed);

	/*Level 2*/
	word = (char*)malloc(strlen("കാര്യമായ") * sizeof(char));
	if(word == NULL)
		ck_abort_msg("Cannot allocate memory");

	stemmed = (char*)malloc(strlen("കാര്യമായ") * sizeof(char));
	if(stemmed == NULL)
		ck_abort_msg("Cannot allocate memory");

	strcpy(word, "കാര്യമായ");

	rc = varnam_create_stemrule(varnam_instance, "മായ", "ം", 2);
	assert_success(rc);

	rc = varnam_stem(varnam_instance, word, false, stemmed);
	assert_success(rc);
	ck_assert_str_eq(stemmed, "കാര്യം");

	/*free(word);
	free(stemmed);*/
	

	/*Level 3*/
	stemmed = (char*)malloc(strlen("പഠിപ്പിക്കുക") * sizeof(char));
	word = (char*)malloc(strlen("പഠിപ്പിക്കുക") * sizeof(char));
	strcpy(word, "പഠിപ്പിക്കുക");

	rc = varnam_create_stemrule(varnam_instance, "പ്പിക്കുക", "പ്പിച്ചു", 3);
	assert_success(rc);

	rc = varnam_stem(varnam_instance, word, false, stemmed);
	assert_success(rc);
	ck_assert_str_eq(stemmed, "പഠിപ്പിച്ചു");

	/*free(stemmed);
	free(word);
	*/	
	rc = varnam_stem(varnam_instance, empty_word, false, stemmed);
	assert_error(rc);
}
END_TEST

START_TEST (stem_with_exceptions)
{
	int rc;
	char *word = "പറയരുതെന്ന്";
	char *stemmed = NULL;

	/*Inserting symbols for vst_get_last_syllable to use inside varnam_check_exception*/
	rc = insert_to_vst(9, "~", "", "", "", "", 1, 0, 0, 0);
	assert_success(rc);
	rc = insert_to_vst(3, "n~", "ന്", "്", "", "", 1, 0, 0, 3);
	assert_success(rc);
	rc = insert_to_vst(2, "na", "ന", "", "", "", 1, 0, 0, 3);
	assert_success(rc);

	stemmed = (char*)malloc(strlen(word) * sizeof(char));
	printf("%s\n", varnam_get_scheme_file(varnam_instance));
	rc = varnam_create_stemrule(varnam_instance, "ന്", "ൻ", 1);
	assert_success(rc);

	rc = varnam_stem(varnam_instance, word, false, stemmed);
	assert_success(rc);
	ck_assert_str_eq(stemmed, "പറയരുതെന്ൻ");

	rc = varnam_create_stem_exception(varnam_instance, "ന്", "ന്");
	assert_success(rc);

	rc = varnam_create_stemrule(varnam_instance, "െന്ന്", "്", 1);
	assert_success(rc);
	
	rc = varnam_stem(varnam_instance, word, false, stemmed);
	ck_assert_str_eq(stemmed, "പറയരുത്");
}
END_TEST

TCase* get_stemmer_tests()
{
 	TCase* tcase = tcase_create("stemmer");
 	tcase_add_checked_fixture(tcase, setup, teardown);
 	tcase_add_checked_fixture(tcase, setup_test_data, NULL);
 	tcase_add_test(tcase, insert_stemrule);
 	tcase_add_test(tcase, syllables);
 	tcase_add_test(tcase, stem_word);
 	tcase_add_test(tcase, stem_with_exceptions);
 	return tcase;
}
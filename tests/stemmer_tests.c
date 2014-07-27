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
#include "../vword.h"

void setup_test_data()
{
	char **msg=NULL;
	char *filename = get_unique_filename();

	reinitialize_varnam_instance(filename);
	ensure_schema_exists(varnam_instance, msg);
}

START_TEST (insert_stemrule)
{
	int rc;
	sqlite3 *db;
	sqlite3_stmt *stmt;
	const char *sql = "select new_ending from stemrules where old_ending = ?1;";
	const char *empty_word=NULL;

	db = varnam_instance->internal->db;

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if(rc != SQLITE_OK)
		ck_abort_msg("Sqlite error : %s", sqlite3_errmsg(db));
	
	rc = varnam_create_stemrule(varnam_instance, empty_word, "ല");
	assert_error(rc);

	rc = varnam_create_stemrule(varnam_instance, "ക", "ല");
	assert_success(rc);

	rc = sqlite3_bind_text(stmt, 1, "ക", -1, NULL);
	if(rc != SQLITE_OK)
		ck_abort_msg("Sqlite error : %s", sqlite3_errmsg(db));

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)
		ck_assert_str_eq((char*)sqlite3_column_text(stmt, 0), "ല");
	else if(rc != SQLITE_DONE)
		ck_abort_msg("Sqlite error : %s", sqlite3_errmsg(db));

	sqlite3_reset(stmt);

	rc = varnam_create_stemrule(varnam_instance, "ളായ", "ൾ");
	assert_success(rc);

	rc = sqlite3_bind_text(stmt, 1, "ളായ", -1, NULL);
	rc = sqlite3_step(stmt);

	if(rc == SQLITE_ROW)
		ck_assert_str_eq((char*)sqlite3_column_text(stmt, 0), "ൾ");
	else if(rc != SQLITE_DONE)
		ck_abort_msg("Sqlite error : %s", sqlite3_errmsg(db));	

	sqlite3_finalize(stmt);
}
END_TEST

START_TEST(stemming)
{
	int rc;
	varray *stem_results = varray_init();
	rc = varnam_create_stemrule(varnam_instance, "മാണ്", "ം");
	assert_success(rc);
	rc = varnam_create_stemrule(varnam_instance, "യാണ്", "");
	assert_success(rc);
	rc = varnam_create_stemrule(varnam_instance, "ന്", "ൻ");
	assert_success(rc);
	rc = varnam_create_stemrule(varnam_instance, "ണെന്ന്", "ണ്");
	assert_success(rc);

	varnam_stem(varnam_instance, "കാര്യമാണ്", stem_results);
	ck_assert_str_eq(((vword*)stem_results->memory[stem_results->index])->text, "കാര്യം");
	varray_clear(stem_results);
	
	varnam_stem(varnam_instance, "അവശതയാണ്", stem_results);
	ck_assert_str_eq(((vword*)stem_results->memory[stem_results->index])->text, "അവശത");
	varray_clear(stem_results);

	rc = varnam_create_stem_exception(varnam_instance, "ന്", "ന്");
	assert_success(rc);
	varnam_stem(varnam_instance, "അവിടെയാണെന്ന്", stem_results);
	ck_assert_str_eq(((vword*)stem_results->memory[stem_results->index])->text, "അവിടെയാണ്");
	varray_clear(stem_results);

	varray_free(stem_results, *destroy_word);
}
END_TEST

TCase* get_stemmer_tests()
{
	TCase* tcase = tcase_create("stemmer");
    tcase_add_checked_fixture(tcase, setup, teardown);
    tcase_add_checked_fixture(tcase, setup_test_data, NULL);
    tcase_add_test(tcase, insert_stemrule);
    tcase_add_test(tcase, stemming);
    return tcase;
}


/* Tests word export
 *
 * Copyright (C) Navaneeth K N
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA */

#include <stdio.h>
#include <string.h>
#include "../varnam.h"
#include <check.h>
#include "testcases.h"

static const char* word1 =  "മലയാളം";
static const char* word2 = "വർണം";
static const char* word3 = "തമിഴ്";

static void
setup_data()
{
    int rc;

    const char *filename = "../schemes/ml-unicode.vst";
    if (!file_exist (filename)) {
        ck_abort_msg ("../schemes/ml-unicode.vst is not available");
    }
    reinitialize_varnam_instance (filename);

    rc = varnam_config (varnam_instance, VARNAM_CONFIG_ENABLE_SUGGESTIONS, get_unique_filename());
    assert_success (rc);

    rc = varnam_learn (varnam_instance, word1);
    assert_success (rc);

    rc = varnam_learn (varnam_instance, word2);
    assert_success (rc);

    rc = varnam_learn (varnam_instance, word3);
    assert_success (rc);
}

START_TEST (varnam_export)
{
    int rc;
    int confidence;
    FILE* fp;
    char buffer[100];


    rc = varnam_export_words (varnam_instance, 2, "output/", NULL);
    assert_success (rc);

    fp = fopen ("output/0.txt", "r");
    if (fp == NULL) {
        ck_abort_msg ("Error opening output/0.txt");
    }

    fscanf (fp, "%s %d", buffer, &confidence);
    ck_assert_str_eq (buffer, word1);

    fscanf (fp, "%s %d", buffer, &confidence);
    ck_assert_str_eq (buffer, word2);

    if (fscanf (fp, "%s %d", buffer, &confidence) == 2) {
        ck_abort_msg ("0.txt has more than 2 lines");
    }

    fclose (fp);

    fp = fopen ("output/1.txt", "r");
    if (fp == NULL) {
        ck_abort_msg ("Error opening output/1.txt");
    }

    fscanf (fp, "%s %d", buffer, &confidence);
    ck_assert_str_eq (buffer, word3);

    if (fscanf (fp, "%s %d", buffer, &confidence) == 1) {
        ck_abort_msg ("0.txt has more than 1 lines");
    }

    fclose (fp);
}
END_TEST

TCase* get_export_tests()
{
    TCase* tcase = tcase_create("export");
    tcase_add_checked_fixture (tcase, setup, teardown);
    tcase_add_checked_fixture (tcase, setup_data, NULL);
    tcase_add_test (tcase, varnam_export);
    return tcase;
}
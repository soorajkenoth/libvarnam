
#include <stdio.h>
#include "testcases.h"
#include "../varnam.h"

static void
enable_suggestions()
{
    varnam_config (varnam_instance, VARNAM_CONFIG_ENABLE_SUGGESTIONS, strbuf_to_s (get_unique_filename()));
}

static void
setup_test_data()
{
    int rc;

    rc = varnam_create_token(varnam_instance, "a", "അ", "", "", "",
            VARNAM_TOKEN_VOWEL, VARNAM_MATCH_EXACT, 0, 0, 0);
    assert_success (rc);

    rc = varnam_create_token(varnam_instance, "aa", "ആ", "ാ", "", "",
            VARNAM_TOKEN_VOWEL, VARNAM_MATCH_EXACT, 0, 0, 0);
    assert_success (rc);

    rc = varnam_create_token(varnam_instance, "a", "ആ", "ാ", "", "",
            VARNAM_TOKEN_VOWEL, VARNAM_MATCH_POSSIBILITY, 0, 0, 0);
    assert_success (rc);

    rc = varnam_create_token(varnam_instance, "ka", "ക", "", "", "",
            VARNAM_TOKEN_CONSONANT, VARNAM_MATCH_EXACT, 0, 0, 0);
    assert_success (rc);

    rc = varnam_create_token(varnam_instance, "kha", "ഖ", "", "", "",
            VARNAM_TOKEN_CONSONANT, VARNAM_MATCH_EXACT, 0, 0, 0);
    assert_success (rc);

    rc = varnam_create_token(varnam_instance, "gha", "ഖ", "", "", "",
            VARNAM_TOKEN_CONSONANT, VARNAM_MATCH_POSSIBILITY, 0, 0, 0);
    assert_success (rc);
}

START_TEST (starting_and_trailing_special_chars_should_be_removed)
{
    int rc;
    rc = varnam_learn (varnam_instance, "''@'കഖഖ@");
    assert_success (rc);
}
END_TEST

START_TEST (words_with_unknown_letters_should_be_rejected)
{
    int rc = varnam_learn (varnam_instance, "test");
    if (rc != VARNAM_ERROR) {
        ck_abort_msg ("Expected return code to be VARNAM_ERROR");
    }
    ck_assert_str_eq (varnam_get_last_error (varnam_instance),
            "Can't process 't'. One or more characters in 'test' are not known\n");
}
END_TEST

START_TEST (basic_learning)
{
    int rc;
    varray *words;
    const char *word_to_learn = "കഖ";

    rc = varnam_learn (varnam_instance, word_to_learn);
    assert_success (rc);

    /* Here gha is a possibility. But since it is learned, it will be suggested back */
    rc = varnam_transliterate (varnam_instance, "kagha", &words);
    assert_success (rc);
    ck_assert_int_eq (varray_length (words), 2);

    ensure_word_list_contains (words, word_to_learn);
}
END_TEST

START_TEST (words_with_repeating_characters_will_not_be_learned)
{
    int rc;
    strbuf *string;
    const char *word_to_learn = "കകകകകകക";

    rc = varnam_learn (varnam_instance, word_to_learn);
    assert_error (rc);
    string = strbuf_init (50);
    strbuf_addf (string, "'%s' looks incorrect. Not learning anything", word_to_learn);
    ck_assert_str_eq (varnam_get_last_error (varnam_instance), strbuf_to_s (string));
}
END_TEST

TCase* get_learning_tests()
{
    TCase* tcase = tcase_create("learning");
    tcase_add_checked_fixture (tcase, setup, teardown);
    tcase_add_checked_fixture (tcase, setup_test_data, NULL);
    tcase_add_checked_fixture (tcase, enable_suggestions, NULL);
    tcase_add_test (tcase, starting_and_trailing_special_chars_should_be_removed);
    tcase_add_test (tcase, words_with_unknown_letters_should_be_rejected);
    tcase_add_test (tcase, basic_learning);
    tcase_add_test (tcase, words_with_repeating_characters_will_not_be_learned);
    return tcase;
}
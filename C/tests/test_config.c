#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../config.h"

#define CONF_PATH "/tmp/test_ssd1306.conf"

static int tests_run    = 0;
static int tests_passed = 0;

#define CHECK(expr, msg) do { \
    tests_run++; \
    if (expr) { \
        printf("  PASS  %s\n", msg); \
        tests_passed++; \
    } else { \
        printf("  FAIL  %s  (line %d)\n", msg, __LINE__); \
    } \
} while(0)

static void reset_config(void)
{
    g_config.show_temperature = 1;
    g_config.show_memory      = 1;
    g_config.show_disk        = 1;
    g_config.temp_fahrenheit  = 1;
    g_config.ip_source        = 0;
    g_config.top_line         = TOP_LINE_IP;
    strncpy(g_config.custom_text, "UCTRONICS", sizeof(g_config.custom_text));
}

static void write_conf(const char *content)
{
    FILE *f = fopen(CONF_PATH, "w");
    fputs(content, f);
    fclose(f);
    setenv("SSD1306_CONF", CONF_PATH, 1);
}

/* ------------------------------------------------------------------ */

static void test_defaults_when_no_file(void)
{
    printf("\ntest_defaults_when_no_file\n");
    unsetenv("SSD1306_CONF");
    reset_config();
    /* point away from any real config files */
    setenv("SSD1306_CONF", "/tmp/does_not_exist_ssd1306.conf", 1);
    load_config();

    CHECK(g_config.show_temperature == 1, "show_temperature defaults to 1");
    CHECK(g_config.show_memory      == 1, "show_memory defaults to 1");
    CHECK(g_config.show_disk        == 1, "show_disk defaults to 1");
    CHECK(g_config.temp_fahrenheit  == 1, "temp_fahrenheit defaults to 1");
    CHECK(g_config.ip_source        == 0, "ip_source defaults to 0 (eth0)");
    CHECK(g_config.top_line         == TOP_LINE_IP, "top_line defaults to TOP_LINE_IP");
    CHECK(strcmp(g_config.custom_text, "UCTRONICS") == 0, "custom_text defaults to UCTRONICS");
}

static void test_show_flags(void)
{
    printf("\ntest_show_flags\n");
    reset_config();
    write_conf(
        "show_temperature=0\n"
        "show_memory=0\n"
        "show_disk=0\n"
    );
    load_config();

    CHECK(g_config.show_temperature == 0, "show_temperature=0");
    CHECK(g_config.show_memory      == 0, "show_memory=0");
    CHECK(g_config.show_disk        == 0, "show_disk=0");
}

static void test_temp_unit(void)
{
    printf("\ntest_temp_unit\n");

    reset_config();
    write_conf("temp_unit=celsius\n");
    load_config();
    CHECK(g_config.temp_fahrenheit == 0, "temp_unit=celsius sets temp_fahrenheit=0");

    reset_config();
    write_conf("temp_unit=fahrenheit\n");
    load_config();
    CHECK(g_config.temp_fahrenheit == 1, "temp_unit=fahrenheit sets temp_fahrenheit=1");
}

static void test_ip_source(void)
{
    printf("\ntest_ip_source\n");

    reset_config();
    write_conf("ip_source=wlan0\n");
    load_config();
    CHECK(g_config.ip_source == 1, "ip_source=wlan0 sets ip_source=1");

    reset_config();
    write_conf("ip_source=eth0\n");
    load_config();
    CHECK(g_config.ip_source == 0, "ip_source=eth0 sets ip_source=0");
}

static void test_top_line(void)
{
    printf("\ntest_top_line\n");

    reset_config();
    write_conf("top_line=hostname\n");
    load_config();
    CHECK(g_config.top_line == TOP_LINE_HOSTNAME, "top_line=hostname");

    reset_config();
    write_conf("top_line=custom\n");
    load_config();
    CHECK(g_config.top_line == TOP_LINE_CUSTOM, "top_line=custom");

    reset_config();
    write_conf("top_line=ip\n");
    load_config();
    CHECK(g_config.top_line == TOP_LINE_IP, "top_line=ip");
}

static void test_custom_text(void)
{
    printf("\ntest_custom_text\n");

    reset_config();
    write_conf("custom_text=MyPi\n");
    load_config();
    CHECK(strcmp(g_config.custom_text, "MyPi") == 0, "custom_text=MyPi");

    reset_config();
    write_conf("custom_text=Hello World\n");
    load_config();
    CHECK(strcmp(g_config.custom_text, "Hello World") == 0, "custom_text with spaces");
}

static void test_comments_and_blank_lines(void)
{
    printf("\ntest_comments_and_blank_lines\n");
    reset_config();
    write_conf(
        "# this is a comment\n"
        "\n"
        "  \n"
        "show_temperature=0\n"
        "# another comment\n"
        "show_memory=0\n"
    );
    load_config();

    CHECK(g_config.show_temperature == 0, "key after comment is parsed");
    CHECK(g_config.show_memory      == 0, "key after second comment is parsed");
    CHECK(g_config.show_disk        == 1, "unmentioned key keeps default");
}

static void test_unknown_keys_ignored(void)
{
    printf("\ntest_unknown_keys_ignored\n");
    reset_config();
    write_conf(
        "unknown_key=foo\n"
        "show_disk=0\n"
    );
    load_config();

    CHECK(g_config.show_disk == 0, "known key after unknown key still parsed");
    CHECK(g_config.show_temperature == 1, "defaults preserved for unmentioned keys");
}

static void test_whitespace_around_equals(void)
{
    printf("\ntest_whitespace_around_equals\n");
    reset_config();
    write_conf(
        "show_temperature = 0\n"
        "temp_unit = celsius\n"
    );
    load_config();

    CHECK(g_config.show_temperature == 0, "spaces around = handled");
    CHECK(g_config.temp_fahrenheit  == 0, "temp_unit with spaces around =");
}

/* ------------------------------------------------------------------ */

int main(void)
{
    test_defaults_when_no_file();
    test_show_flags();
    test_temp_unit();
    test_ip_source();
    test_top_line();
    test_custom_text();
    test_comments_and_blank_lines();
    test_unknown_keys_ignored();
    test_whitespace_around_equals();

    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}

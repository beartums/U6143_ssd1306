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
    g_config.show_clock       = 0;
    g_config.show_uptime      = 0;
    g_config.show_cpu_freq    = 0;
    g_config.show_gpu_temp    = 0;
    g_config.show_network     = 0;
    g_config.show_wifi        = 0;
    g_config.show_docker      = 0;
    g_config.show_hostname    = 0;
    g_config.show_ip          = 0;
    g_config.temp_fahrenheit  = 1;
    g_config.load_cores       = 0;
    g_config.top_line         = TOP_LINE_IP;
    strncpy(g_config.custom_text,       "UCTRONICS",  sizeof(g_config.custom_text));
    strncpy(g_config.network_interfaces, "eth0,wlan0", sizeof(g_config.network_interfaces));
    g_config.screen_time      = 3;
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
    CHECK(g_config.show_clock       == 0, "show_clock defaults to 0");
    CHECK(g_config.show_uptime      == 0, "show_uptime defaults to 0");
    CHECK(g_config.show_cpu_freq    == 0, "show_cpu_freq defaults to 0");
    CHECK(g_config.show_gpu_temp    == 0, "show_gpu_temp defaults to 0");
    CHECK(g_config.show_network     == 0, "show_network defaults to 0");
    CHECK(g_config.show_wifi        == 0, "show_wifi defaults to 0");
    CHECK(g_config.show_docker      == 0, "show_docker defaults to 0");
    CHECK(g_config.show_hostname    == 0, "show_hostname defaults to 0");
    CHECK(g_config.show_ip          == 0, "show_ip defaults to 0");
    CHECK(g_config.temp_fahrenheit  == 1, "temp_fahrenheit defaults to 1");
    CHECK(g_config.load_cores       == 0, "load_cores defaults to 0 (percent)");
    CHECK(strcmp(g_config.network_interfaces, "eth0,wlan0") == 0, "network_interfaces defaults to eth0,wlan0");
    CHECK(g_config.screen_time == 3,                       "screen_time defaults to 3");
    CHECK(g_config.top_line == TOP_LINE_IP,                "top_line defaults to TOP_LINE_IP");
    CHECK(strcmp(g_config.custom_text, "UCTRONICS") == 0,  "custom_text defaults to UCTRONICS");
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
    printf("\ntest_ip_source (backwards-compat alias)\n");

    reset_config();
    write_conf("ip_source=wlan0\n");
    load_config();
    CHECK(strcmp(g_config.network_interfaces, "wlan0,eth0") == 0,
          "ip_source=wlan0 sets network_interfaces to wlan0,eth0");

    reset_config();
    write_conf("ip_source=eth0\n");
    load_config();
    CHECK(strcmp(g_config.network_interfaces, "eth0,wlan0") == 0,
          "ip_source=eth0 sets network_interfaces to eth0,wlan0");
}

static void test_network_interfaces(void)
{
    printf("\ntest_network_interfaces\n");

    reset_config();
    write_conf("network_interfaces=wlan0,eth0\n");
    load_config();
    CHECK(strcmp(g_config.network_interfaces, "wlan0,eth0") == 0,
          "network_interfaces=wlan0,eth0");

    reset_config();
    write_conf("network_interfaces=eth0\n");
    load_config();
    CHECK(strcmp(g_config.network_interfaces, "eth0") == 0,
          "network_interfaces=eth0 (single interface)");
}

static void test_load_display(void)
{
    printf("\ntest_load_display\n");

    reset_config();
    write_conf("load_display=cores\n");
    load_config();
    CHECK(g_config.load_cores == 1, "load_display=cores sets load_cores=1");

    reset_config();
    write_conf("load_display=percent\n");
    load_config();
    CHECK(g_config.load_cores == 0, "load_display=percent sets load_cores=0");
}

static void test_screen_time(void)
{
    printf("\ntest_screen_time\n");

    reset_config();
    write_conf("screen_time=5\n");
    load_config();
    CHECK(g_config.screen_time == 5, "screen_time=5");

    reset_config();
    write_conf("screen_time=1\n");
    load_config();
    CHECK(g_config.screen_time == 1, "screen_time=1");
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

    reset_config();
    write_conf("top_line=none\n");
    load_config();
    CHECK(g_config.top_line == TOP_LINE_NONE, "top_line=none");

    reset_config();
    write_conf("top_line=unknown\n");
    load_config();
    CHECK(g_config.top_line == TOP_LINE_IP, "unknown top_line falls back to ip");
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

static void test_new_show_flags(void)
{
    printf("\ntest_new_show_flags\n");
    reset_config();
    write_conf(
        "show_clock=1\n"
        "show_uptime=1\n"
        "show_cpu_freq=1\n"
        "show_gpu_temp=1\n"
        "show_network=1\n"
        "show_wifi=1\n"
        "show_docker=1\n"
        "show_hostname=1\n"
        "show_ip=1\n"
    );
    load_config();

    CHECK(g_config.show_clock    == 1, "show_clock=1");
    CHECK(g_config.show_uptime   == 1, "show_uptime=1");
    CHECK(g_config.show_cpu_freq == 1, "show_cpu_freq=1");
    CHECK(g_config.show_gpu_temp == 1, "show_gpu_temp=1");
    CHECK(g_config.show_network  == 1, "show_network=1");
    CHECK(g_config.show_wifi     == 1, "show_wifi=1");
    CHECK(g_config.show_docker   == 1, "show_docker=1");
    CHECK(g_config.show_hostname == 1, "show_hostname=1");
    CHECK(g_config.show_ip       == 1, "show_ip=1");

    /* verify existing screens unaffected */
    CHECK(g_config.show_temperature == 1, "show_temperature still default");
    CHECK(g_config.show_memory      == 1, "show_memory still default");
    CHECK(g_config.show_disk        == 1, "show_disk still default");
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
    test_network_interfaces();
    test_load_display();
    test_screen_time();
    test_top_line();
    test_custom_text();
    test_comments_and_blank_lines();
    test_unknown_keys_ignored();
    test_new_show_flags();
    test_whitespace_around_equals();

    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}

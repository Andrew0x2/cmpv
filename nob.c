#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "gcc");
    nob_cmd_append(&cmd, "-Wall", "-Wextra", "-ljson-c");
    nob_cmd_append(&cmd, "-o", "mpv_monitor");
    nob_cmd_append(&cmd, "mpv_monitor.c");

    if (!nob_cmd_run_sync(cmd)) {exit(1);}
    if (cmd.items) {free(cmd.items);}
    return 0;

}

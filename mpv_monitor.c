#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <json-c/json.h>
#include <stdbool.h>
#include <fcntl.h>


#define MPV_SOCKET "/tmp/mpvsocket"
#define BUFFER_SIZE 4096

// Definizione delle strutture
typedef struct {
    int fd;
    char buffer[BUFFER_SIZE];
} MPVConnection;

typedef struct {
    char* title;
    char* filename;
    double time_pos;
    double duration;
} MPVTrackInfo;

// Dichiarazioni di funzioni
MPVConnection* mpv_connect(void);
MPVConnection* safe_mpv_connect(void);
void mpv_disconnect(MPVConnection* conn);
bool is_mpv_running(void);
int mpv_send_command(MPVConnection* conn, const char* command);
char* mpv_read_response(MPVConnection* conn);
char* parse_json_value(const char* json_str, const char* key);
MPVTrackInfo* get_mpv_track_info(MPVConnection* conn);
void print_track_info(const MPVTrackInfo* info);
void free_track_info(MPVTrackInfo* info);

// Gestione degli errori migliorata
void handle_error(const char* message) {
    fprintf(stderr, "ERRORE CRITICO: %s - Dettaglio: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

// Connessione al socket MPV con maggiore robustezza
MPVConnection* mpv_connect(void) {
    if (!is_mpv_running()) {
        return NULL;
    }

    MPVConnection* conn = calloc(1, sizeof(MPVConnection));
    if (!conn) {
        handle_error("Allocazione memoria connessione fallita");
        return NULL;
    }

    conn->fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (conn->fd == -1) {
        free(conn);
        handle_error("Creazione socket fallita");
        return NULL;
    }

    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = MPV_SOCKET
    };

    if (connect(conn->fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(conn->fd);
        free(conn);
        handle_error("Connessione al socket fallita");
        return NULL;
    }

    return conn;
}

MPVConnection* safe_mpv_connect(void) {
    if (!is_mpv_running()) {
        printf("🚫 - Mpv is not executed");
        return NULL;
    }
    return mpv_connect();
}

// Disconnessione e pulizia
void mpv_disconnect(MPVConnection* conn) {
    if (conn) {
        close(conn->fd);
        free(conn);
    }
}

// Verifica se MPV è in esecuzione
bool is_mpv_running(void) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) return false;

    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
    };
    strncpy(addr.sun_path, MPV_SOCKET, sizeof(addr.sun_path) - 1);

    int result = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    close(fd);

    return (result == 0 || (result == -1 && errno == EINPROGRESS));
}

// Invio comando a MPV
int mpv_send_command(MPVConnection* conn, const char* command) {
    if (!conn) return 0;
    
    ssize_t bytes_sent = write(conn->fd, command, strlen(command));
    return (bytes_sent != -1);
}

// Lettura risposta da MPV
char* mpv_read_response(MPVConnection* conn) {
    if (!conn) return NULL;

    memset(conn->buffer, 0, BUFFER_SIZE);
    ssize_t bytes_read = read(conn->fd, conn->buffer, BUFFER_SIZE - 1);

    return (bytes_read > 0) ? strdup(conn->buffer) : NULL;
}

// Parsing JSON migliorato
char* parse_json_value(const char* json_str, const char* key) {
    struct json_object *parsed_json, *value;
    char* result = NULL;

    parsed_json = json_tokener_parse(json_str);
    if (!parsed_json) return NULL;

    if (json_object_object_get_ex(parsed_json, key, &value)) {
        result = strdup(json_object_get_string(value));
    }

    json_object_put(parsed_json);
    return result;
}

// Recupero informazioni traccia
MPVTrackInfo* get_mpv_track_info(MPVConnection* conn) {
    const char* properties[] = {"media-title", "path", "time-pos", "duration"};
    MPVTrackInfo* info = calloc(1, sizeof(MPVTrackInfo));

    for (int i = 0; i < 4; i++) {
        char command[256];
        snprintf(command, sizeof(command),
                 "{\"command\": [\"get_property\", \"%s\"]}\n",
                 properties[i]);

        if (mpv_send_command(conn, command)) {
            char* response = mpv_read_response(conn);
            if (response) {
                char* value = parse_json_value(response, "data");
                if (value) {
                    switch(i) {
                        case 0: info->title = value; break;
                        case 1: info->filename = value; break;
                        case 2: info->time_pos = atof(value); free(value); break;
                        case 3: info->duration = atof(value); free(value); break;
                    }
                }
                free(response);
            }
        }
    }

    return info;
}

// Stampa informazioni traccia
void print_track_info(const MPVTrackInfo* info) {
    if (!info || !info->title) {
        printf("🚫 - No Music");
        return;
    }

    double percentage = (info->time_pos / info->duration) * 100;
    printf("%s [%.1f%%]", info->title, percentage);
}

// Liberazione memoria
void free_track_info(MPVTrackInfo* info) {
    if (info) {
        free(info->title);
        free(info->filename);
        free(info);
    }
}

// Funzione principale
int main(void) {
    MPVConnection* conn = safe_mpv_connect();
    if (!conn) {
        return EXIT_SUCCESS;
    }

    MPVTrackInfo* track_info = get_mpv_track_info(conn);
    print_track_info(track_info);

    free_track_info(track_info);
    mpv_disconnect(conn);

    return EXIT_SUCCESS;
}

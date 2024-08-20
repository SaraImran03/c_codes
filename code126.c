#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

struct connection_info_struct {
    char *username;
    struct MHD_PostProcessor *post_processor;
};

static int iterate_post(void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
                        const char *filename, const char *content_type,
                        const char *transfer_encoding, const char *data, uint64_t off, size_t size) {
    struct connection_info_struct *con_info = coninfo_cls;

    if (0 == strcmp(key, "username")) {
        // Allocate memory for and copy the username
        if (size > 0 && size < 1024) {
            con_info->username = strndup(data, size);
            return MHD_YES;
        } else {
            return MHD_NO;
        }
    }

    return MHD_YES;
}

static void request_completed(void *cls, struct MHD_Connection *connection,
                              void **con_cls, enum MHD_RequestTerminationCode toe) {
    struct connection_info_struct *con_info = *con_cls;

    if (con_info == NULL)
        return;

    if (con_info->post_processor) {
        MHD_destroy_post_processor(con_info->post_processor);
    }
    if (con_info->username) {
        free(con_info->username);
    }
    free(con_info);
    *con_cls = NULL;
}

static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    if (strcmp(url, "/hello") == 0 && strcmp(method, "POST") == 0) {
        if (*con_cls == NULL) {
            struct connection_info_struct *con_info;

            con_info = malloc(sizeof(struct connection_info_struct));
            if (con_info == NULL)
                return MHD_NO;
            con_info->username = NULL;

            con_info->post_processor = MHD_create_post_processor(connection, 1024, iterate_post, (void *)con_info);

            if (con_info->post_processor == NULL) {
                free(con_info);
                return MHD_NO;
            }

            *con_cls = (void *)con_info;

            return MHD_YES;
        }

        struct connection_info_struct *con_info = *con_cls;

        if (*upload_data_size != 0) {
            MHD_post_process(con_info->post_processor, upload_data, *upload_data_size);
            *upload_data_size = 0;
            return MHD_YES;
        } else {
            char *outputbuf;
            int ret;
            struct MHD_Response *response;

            asprintf(&outputbuf, "<html><body>Hello, %s!</body></html>", con_info->username ? con_info->username : "Guest");
            response = MHD_create_response_from_buffer(strlen(outputbuf), outputbuf, MHD_RESPMEM_MUST_FREE);
            ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
            MHD_destroy_response(response);
            return ret;
        }
    }

    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL, MHD_OPTION_END);
    if (NULL == daemon)
        return 1;

    getchar();  // Press Enter to stop the server

    MHD_stop_daemon(daemon);
    return 0;
}

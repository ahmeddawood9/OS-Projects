#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "request.h"
#include "io_helper.h"

char default_root[] = ".";

typedef struct {
    int fd;
    int size;
    long arrival;
} request_t;

request_t *buffer;
int buffer_size;
int count = 0;
long arrival_counter = 0;
int is_sff = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill_cv = PTHREAD_COND_INITIALIZER;

void put(int fd, int size) {
    for (int i = 0; i < buffer_size; i++) {
        if (buffer[i].fd == -1) {
            buffer[i].fd = fd;
            buffer[i].size = size;
            buffer[i].arrival = arrival_counter++;
            count++;
            break;
        }
    }
}

int get() {
    int best_idx = -1;
    for (int i = 0; i < buffer_size; i++) {
        if (buffer[i].fd != -1) {
            if (best_idx == -1) {
                best_idx = i;
            } else {
                if (is_sff) {
                    if (buffer[i].size < buffer[best_idx].size)
                        best_idx = i;
                } else {
                    if (buffer[i].arrival < buffer[best_idx].arrival)
                        best_idx = i;
                }
            }
        }
    }
    int fd = buffer[best_idx].fd;
    buffer[best_idx].fd = -1;
    count--;
    return fd;
}

void *worker(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock);
        while (count == 0)
            pthread_cond_wait(&fill_cv, &lock);
        int fd = get();
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&lock);
        request_handle(fd);
        close_or_die(fd);
    }
    return NULL;
}

int get_request_size(int fd) {
    char buf[8192];
    ssize_t n = recv(fd, buf, sizeof(buf) - 1, MSG_PEEK);
    if (n <= 0) return 0;
    buf[n] = '\0';
    
    char *end = strstr(buf, "\r\n");
    if (!end) end = strstr(buf, "\n");
    if (!end) return 0;
    
    char line[8192];
    int len = end - buf;
    strncpy(line, buf, len);
    line[len] = '\0';
    
    char method[1024], uri[1024], version[1024];
    if (sscanf(line, "%s %s %s", method, uri, version) != 3) return 0;
    
    char filename[1024], cgiargs[1024];
    request_parse_uri(uri, filename, cgiargs);
    
    struct stat sbuf;
    if (stat(filename, &sbuf) < 0) return 0;
    
    return sbuf.st_size;
}

//
// ./wserver [-d <basedir>] [-p <portnum>] [-t <threads>] [-b <buffers>] [-s <schedalg>]
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
    int threads = 1;
    int buffers = 1;
    char *schedalg = "FIFO";
    
    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1)
	switch (c) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 't':
	    threads = atoi(optarg);
	    break;
	case 'b':
	    buffers = atoi(optarg);
	    break;
	case 's':
	    schedalg = optarg;
	    break;
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]\n");
	    exit(1);
	}

    if (strcmp(schedalg, "SFF") == 0) {
        is_sff = 1;
    }

    // run out of this directory
    chdir_or_die(root_dir);

    buffer_size = buffers;
    buffer = malloc(sizeof(request_t) * buffer_size);
    for (int i = 0; i < buffer_size; i++) {
        buffer[i].fd = -1;
    }

    pthread_t *pool = malloc(sizeof(pthread_t) * threads);
    for (int i = 0; i < threads; i++) {
        pthread_create(&pool[i], NULL, worker, NULL);
    }

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr);
	int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
        
        int size = 0;
        if (is_sff) {
            size = get_request_size(conn_fd);
        }

        pthread_mutex_lock(&lock);
        while (count == buffer_size)
            pthread_cond_wait(&empty, &lock);
        put(conn_fd, size);
        pthread_cond_signal(&fill_cv);
        pthread_mutex_unlock(&lock);
    }
    return 0;
}


    


 

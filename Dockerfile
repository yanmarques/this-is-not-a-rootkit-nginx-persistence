from debian:11

run apt update && apt install -y \
        gcc unzip libpcre2-dev zlib1g-dev make curl

arg version=1.24
workdir /opt/
run curl -s -o ./nginx.zip \
        https://codeload.github.com/nginx/nginx/zip/refs/heads/branches/stable-${version} && \
    unzip -d ./nginx-tmp-src/ ./nginx.zip && \
    mv ./nginx-tmp-src/*/ ./nginx-src/

copy ./persistence-module/ /opt/persistence-module/
copy ./container/scripts/* /opt/nginx-src/

workdir /opt/nginx-src/

cmd ["/opt/nginx-src/build", "-c"]

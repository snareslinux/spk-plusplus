FROM fedora:latest

RUN dnf install -y \
        g++ \
        gcc \
        make \
        libarchive-devel \
        libcurl-devel \
        zstd \
        gdb \
    && dnf clean all

WORKDIR /src

CMD ["/bin/bash"]
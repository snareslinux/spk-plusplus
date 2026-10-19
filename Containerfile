FROM registry.access.redhat.com/ubi10/ubi:latest

RUN dnf install -y \
        gcc \
        make \
        libarchive-devel \
        libcurl-devel \
        zstd \
        gdb \
    && dnf clean all

WORKDIR /src

CMD ["/bin/bash"]
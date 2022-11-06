FROM ubuntu:20.04 AS qemu-builder

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        git \
        curl \
        wget

RUN apt-get install -y --no-install-recommends \
      build-essential \
      git \
      libglib2.0-dev \
      libfdt-dev \
      libpixman-1-dev \
      zlib1g-dev \
      ninja-build

WORKDIR /data
COPY ../../qemu-portux ./qemu-portux

WORKDIR /data/build
RUN ../qemu-portux/configure --target-list=arm-softmmu --disable-werror
RUN make -j$(nproc)


FROM ubuntu:20.04 AS qemu-runner

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        git \
        curl \
        wget

RUN apt-get install -y --no-install-recommends \
      build-essential \
      git \
      libglib2.0-dev \
      libfdt-dev \
      libpixman-1-dev \
      zlib1g-dev \
      ninja-build

COPY --from=qemu-builder /data/build/arm-softmmu/qemu-system-arm /usr/local/bin/qemu-system-arm

ENTRYPOINT ["qemu-system-arm"]
CMD ["-nographic", "-M", "portux920t", "-m", "64M", "-kernel", "/kernel"]
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    python3 \
    && rm -rf /var/lib/apt/lists/*

RUN wget https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v13.2.0-1/xpack-riscv-none-elf-gcc-13.2.0-1-linux-x64.tar.gz \
 && tar -xzf xpack-riscv-none-elf-gcc-13.2.0-1-linux-x64.tar.gz \
 && mv xpack-riscv-none-elf-gcc-13.2.0-1 /opt/riscv \
 && rm xpack-riscv-none-elf-gcc-13.2.0-1-linux-x64.tar.gz

ENV PATH="/opt/riscv/bin:${PATH}"

WORKDIR /work
CMD ["/bin/bash"]


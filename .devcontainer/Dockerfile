ARG VARIANT=ubuntu-22.04
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive && \
    apt-get -y install --no-install-recommends \
        lsb-release \
        software-properties-common

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.23.1"
COPY ./reinstall-cmake.sh /tmp/
RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

ARG GCC_VERSION="12"
RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get -y install gcc-12 g++-12 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_VERSION} ${GCC_VERSION} && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-${GCC_VERSION} ${GCC_VERSION}

ARG CLANG_VERSION="16"
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
RUN wget -O - https://apt.llvm.org/llvm.sh > /tmp/llvm.sh && \
    chmod +x /tmp/llvm.sh && \
    export DEBIAN_FRONTEND=noninteractive && \
    /tmp/llvm.sh ${CLANG_VERSION} && \
    update-alternatives --remove-all gcc && \
    update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${CLANG_VERSION} ${CLANG_VERSION} && \
    update-alternatives --set cc /usr/bin/clang && \
    update-alternatives --remove-all g++ && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${CLANG_VERSION} ${CLANG_VERSION} && \
    update-alternatives --set c++ /usr/bin/clang++ && \
    apt-get -y install clang-tidy-${CLANG_VERSION} && \
    update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-${CLANG_VERSION} ${CLANG_VERSION}

RUN su vscode -c "${VCPKG_ROOT}/vcpkg install \
    fmt \
    range-v3 \
    libunifex \
"

RUN git clone https://github.com/NVIDIA/stdexec.git /usr/src/stdexec && \
    cmake -S /usr/src/stdexec/ -B /tmp/stdexec/build/ && \
    cmake --build /tmp/stdexec/build/ --target install

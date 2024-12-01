FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg:latest

RUN apt-get -qq update && \
    apt-get -qq install -y g++ make binutils cmake libssl-dev libboost-system-dev libcurl4-openssl-dev zlib1g-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/tgbot-cpp
COPY third_party/tgbot-cpp/include include
COPY third_party/tgbot-cpp/src src
COPY third_party/tgbot-cpp/CMakeLists.txt ./

RUN cmake . && \
    make -j$(nproc) && \
    make install && \
    rm -rf /usr/src/tgbot-cpp/*

WORKDIR /usr/src/bot
COPY . .
RUN cmake .
RUN make -j4
CMD ./never_forget_bot
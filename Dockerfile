# Use an official Ubuntu as a parent image
FROM ubuntu:22.04

# Set environment variables to suppress prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libpqxx-dev \
    libcurl4-openssl-dev \
    libssl-dev \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Install tgbot-cpp from source
RUN git clone https://github.com/reo7sp/tgbot-cpp.git /opt/tgbot-cpp && \
    cd /opt/tgbot-cpp && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make && make install

# Update linker cache
RUN ldconfig

# Create app directory
WORKDIR /usr/src/app

# Copy CMakeLists and source code
COPY CMakeLists.txt ./
COPY src/ src/

# Build the application
RUN cmake .
RUN make

# Set the entrypoint
CMD ["./TelegramBot"]
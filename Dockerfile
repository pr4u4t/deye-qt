# Use a base Debian image
FROM debian:bookworm

# Set working directory
WORKDIR /app

# Install dependencies
RUN apt update && apt install -y \
    qtbase6-dev \
    qtserialport6-dev \
    qtserialbus6-dev \
    qt6-base-dev \
    qt6-httpserver-dev \
    qt6-tools-dev \
    qtchooser \
    qt6-qmake \
    g++ \
    make \
    git \
    && rm -rf /var/lib/apt/lists/*

# Copy application source code
COPY . /app/

# Build the application using qmake
RUN qmake6 CONFIG+=release && \
    make #-j$(nproc)

# Expose the port for HTTP communication
EXPOSE 8080

# Run the application
CMD ["/app/src/deye", "-d", "ttyUSB0", "-p", "none", "-b", "9600", "-l", "8", "-s", "1", "-t", "1000", "-r", "3", "--loop"]

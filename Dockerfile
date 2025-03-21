FROM carlonluca/qt-dev:6.8.2

WORKDIR /app
COPY . /app/

RUN qmake6 CONFIG+=release && \
    make

EXPOSE 8080
CMD ["/app/src/deye", "-d", "ttyUSB0", "-p", "none", "-b", "9600", "-l", "8", "-s", "1", "-t", "1000", "-r", "3", "--loop"]

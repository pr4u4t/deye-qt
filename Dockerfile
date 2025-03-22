FROM carlonluca/qt-dev:6.8.2

WORKDIR /app
COPY . /app/

RUN qmake6 CONFIG+=release && \
    make

EXPOSE 8080
ENTRYPOINT ["/app/src/deye"]
CMD ["-c", "/app/config.js"]


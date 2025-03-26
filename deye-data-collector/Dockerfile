
FROM carlonluca/qt-dev:6.8.2

LABEL org.opencontainers.image.source https://github.com/pr4u4t/deye-qt

VOLUME [ "/data" ]

WORKDIR /app
COPY . /app/

RUN qmake6 CONFIG+=release && \
    make

#RUN chmod +x /app/run.sh

ENTRYPOINT ["/app/src/deye"]
CMD ["-c", "/data/options.json", "--loop"]

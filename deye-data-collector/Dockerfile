
FROM carlonluca/qt-dev:6.8.2

LABEL org.opencontainers.image.source https://github.com/pr4u4t/deye-qt

WORKDIR /app
COPY . /app/

RUN qmake6 CONFIG+=release && \
    make

ENTRYPOINT ["/app/run.sh"]

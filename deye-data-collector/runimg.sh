#!/bin/bash

docker run -d  -p 8080:8080 --device=/dev/ttyUSB0 deye-app

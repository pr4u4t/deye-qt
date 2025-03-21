#!/bin/bash

docker run -p 8080:8080 --device=/dev/ttyUSB0 deye-app

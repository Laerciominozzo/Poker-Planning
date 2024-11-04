#!/bin/bash

set -e

case $1 in
    build)
        docker-compose run --rm build
        ;;
    clean)
       docker-compose run --rm clean

        ;;
    run)
        docker-compose run --rm --service-ports run
        ;;
    debug)
        docker-compose run --rm --service-ports debug
        ;;
    *)
        echo "Usage: $0 {build|clean|run}"
        exit 1
        ;;
esac
FROM golang:latest as build-app
ADD . /src
WORKDIR /src
RUN go get
RUN CGO_ENABLED=0 go build -o server . 

FROM golang:latest as build-yandex-weather-cli
RUN CGO_ENABLED=0 go get -u github.com/msoap/yandex-weather-cli

FROM alpine:latest
RUN apk --no-cache add ca-certificates

COPY --from=build-app /src/server /app/server
COPY --from=build-app /src/www /app/www
COPY --from=build /usr/local/go/lib /usr/local/go/lib

COPY --from=build-yandex-weather-cli /go/bin/yandex-weather-cli /bin

EXPOSE 3000
WORKDIR /app
HEALTHCHECK --start-period=30s CMD curl -f http://localhost:3000/api/weather || exit 1
CMD ["/app/server"]
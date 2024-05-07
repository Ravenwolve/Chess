# пока что не несёт практической ценности, чисто проба технологии

FROM alpine
WORKDIR /app
COPY /build/Chess /app/Chess
RUN apk add libc6-compat && \
    apk add build-base
ENTRYPOINT ["./Chess"]
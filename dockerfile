FROM golang:latest as colour-agg-builder

WORKDIR /app
COPY ./tools/colour-agg/go.mod . 
RUN go mod download

COPY ./tools/colour-agg/*.go ./

RUN go build -o main .

##################################################

FROM devkitpro/devkitarm:20200730 as GBA-builder

# Needed for creating assets
RUN apt-get update \
	&& apt-get install -y imagemagick

COPY --from=colour-agg-builder /app/main /bin/colour-agg.exe

RUN mkdir /app

WORKDIR /app

ENTRYPOINT [ "make" ]
CMD [ "build" ]
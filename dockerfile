FROM golang:latest as colour-agg-builder

WORKDIR /app
COPY ./tools/colour-agg/go.mod . 
RUN go mod download

COPY ./tools/colour-agg/*.go ./

RUN go build -o main .

##################################################

#Devkit pro image is out of date also deabain is fucked here for some reason
FROM devkitpro/devkitarm:latest as GBA-builder

# Needed for creating assets
RUN apt-get update \
	&& apt-get install -y imagemagick

COPY --from=colour-agg-builder /app/main /bin/colour-agg.exe

RUN mkdir /app

WORKDIR /app

COPY build.sh .

RUN chmod +x build.sh

ENTRYPOINT [ "./build.sh" ]
CMD [ "build" ]
FROM golang:latest as colour-agg-builder

WORKDIR /app
COPY ./tools/colour-agg/go.mod . 
RUN go mod download

COPY ./tools/colour-agg/*.go ./

RUN go build -o main .

############################################

FROM golang:latest as builder-builder

WORKDIR /app
COPY ./tools/builder/go.mod . 
RUN go mod download

COPY ./tools/builder/*.go ./

RUN go build -o main .

##################################################

FROM golang:latest as version-img-gen-builder

WORKDIR /app
COPY ./tools/version-img-gen/go.mod . 
COPY ./tools/version-img-gen/go.sum . 
RUN go mod download

COPY ./tools/version-img-gen/*.go ./

RUN go build -o main .

##################################################

#Devkit pro image is out of date also deabain is fucked here for some reason
FROM devkitpro/devkitarm:latest as GBA-builder

# Needed for creating assets
RUN apt-get update \
	&& apt-get install -y imagemagick

#Copy colour agg tool
COPY --from=colour-agg-builder /app/main /bin/colour-agg.exe

#Copy builder
COPY --from=builder-builder /app/main /bin/builder
RUN chmod +x /bin/builder

#Copy version-img-gen-builder
COPY --from=version-img-gen-builder /app/main /bin/version-img-gen
RUN chmod +x /bin/version-img-gen

RUN mkdir /app

WORKDIR /app

ENTRYPOINT [ "builder" ]
CMD [ "build" ]
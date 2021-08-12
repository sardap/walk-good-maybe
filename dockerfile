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
COPY ./tools/builder/go.sum . 
RUN go mod download

COPY ./tools/builder/*.go ./
COPY ./tools/builder/assets/*.go ./assets/
COPY ./tools/builder/gbacolour/*.go ./gbacolour/


RUN go build -o main .

##################################################

#Devkit pro image is out of date also deabain is fucked here for some reason
FROM devkitpro/devkitarm:latest as GBA-builder

#Copy colour agg tool
COPY --from=colour-agg-builder /app/main /bin/colour-agg.exe

#Copy builder
COPY --from=builder-builder /app/main /bin/builder
RUN chmod +x /bin/builder

RUN mkdir /app

WORKDIR /app

ENTRYPOINT [ "builder" ]
CMD [ "/app/build.toml", "/app/assets", "/app/source/assets", "build" ]
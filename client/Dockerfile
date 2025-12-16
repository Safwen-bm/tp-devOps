# --- ÉTAPE 1: Build Stage (Compilation) ---
FROM gcc:11 AS build
WORKDIR /app
COPY . .

RUN gcc -o client -static client.c

# --- ÉTAPE 2: Final Stage (Image Légère) ---
FROM alpine:latest
WORKDIR /usr/local/bin
COPY --from=build /app/client .

CMD ["/usr/local/bin/client", "10.108.181.11", "6000"]

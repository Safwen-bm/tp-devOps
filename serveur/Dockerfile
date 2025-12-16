# --- ÉTAPE 1: Build Stage (Compilation) ---
FROM gcc:11 AS build
WORKDIR /app
COPY . .

RUN gcc -o serveur -static serveur.c

# --- ÉTAPE 2: Final Stage (Image Légère) ---
FROM alpine:latest
WORKDIR /usr/local/bin

COPY --from=build /app/serveur .

CMD ["./serveur", "6000"]

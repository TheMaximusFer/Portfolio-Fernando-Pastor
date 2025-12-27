# Usamos una imagen base con Java 17 
FROM eclipse-temurin:17-jdk-alpine

# Definimos el directorio de trabajo dentro del contenedor
WORKDIR /app

COPY target/EnderArchive-0.0.1-SNAPSHOT.jar app.jar

EXPOSE 8080

ENTRYPOINT ["java", "-jar", "app.jar"]
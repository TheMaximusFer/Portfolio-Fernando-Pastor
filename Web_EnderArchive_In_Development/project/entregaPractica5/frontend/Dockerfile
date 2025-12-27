FROM node:22-alpine as build

WORKDIR /app

# Copiamos package.json e instalamos dependencias
COPY package*.json ./
RUN npm install

# Copiamos el resto del código y construimos
COPY . .
RUN npm run build

# --- ETAPA 2: SERVIDOR (Nginx) ---
FROM nginx:alpine

# Copiamos la configuración de Nginx que creamos
COPY nginx.conf /etc/nginx/conf.d/default.conf

# Copiamos los archivos construidos desde la etapa 'build' a la carpeta de Nginx
# IMPORTANTE: Como usamos Vite, ponemos el directorio /app/dist
COPY --from=build /app/dist /usr/share/nginx/html

EXPOSE 80
CMD ["nginx", "-g", "daemon off;"]
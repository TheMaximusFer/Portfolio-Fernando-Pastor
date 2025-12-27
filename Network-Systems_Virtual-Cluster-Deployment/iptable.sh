#!/bin/sh
#INICIAMOS TABLA Y POLÍTICAS
iptables -t nat -F
iptables -F
iptables -P INPUT DROP
iptables -P FORWARD DROP
iptables -P OUTPUT ACCEPT

#BUCLE LOCAL
iptables -A INPUT -i lo -j ACCEPT
iptables -A OUTPUT -o lo -j ACCEPT

#PERMITIMOS TODA CONEXIÓN ENTRE NODOS DE LA INTRANET
iptables -A FORWARD -i enp0s9 -j ACCEPT
iptables -A FORWARD -i enp0s10 -j ACCEPT
iptables -A INPUT -i enp0s9 -j ACCEPT
iptables -A INPUT -i enp0s10 -j ACCEPT

#PERMITIMOS TRÁFICO DE SALIDA
#DESDE LA INTRANET HACIA INTERNET
iptables -A FORWARD -i enp0s9 -o enp0s3 -j ACCEPT
iptables -A FORWARD -i enp0s10 -o enp0s3 -j ACCEPT
#DESDE LA INTRANET HACIA HOST-ONLY
iptables -A FORWARD -i enp0s9 -o enp0s8 -j ACCEPT
iptables -A FORWARD -i enp0s10 -o enp0s8 -j ACCEPT

#PERMITIMOS RESPUESTAS DE LA EXTRANET, SOLO PARA CONEXIONES YA ESTABLECIDAS POR LA INTRANET
iptables -A INPUT  -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT

#PERMITIMOS TRÁFICO HACIA EL SERVIDOR WEB Y EL SERVIDOR SSH RESPECTIVAMENTE
iptables -A FORWARD -d 10.0.1.2 -p tcp --dport 80 -m state --state NEW,ESTABLISHED -j ACCEPT
iptables -A FORWARD -d 10.0.1.2 -p tcp --dport 443 -m state --state NEW,ESTABLISHED -j ACCEPT
iptables -A FORWARD -d 10.0.3.2 -p tcp --dport 22 -m state --state NEW,ESTABLISHED -j ACCEPT

#NAT
#IP DE SALIDA COMO DIRECCIÓN PÚBLICA DEL FIREWALL.
iptables -t nat -A POSTROUTING -o enp0s3 -j SNAT --to 192.168.56.2
iptables -t nat -A POSTROUTING -o enp0s8 -j SNAT --to 192.168.56.2 
#IP DE ENTRADA CAMBIA DEPENDIENDO DEL PUERTO DEL PAQUETE
iptables -t nat -A PREROUTING -i enp0s8 -p tcp --dport 80 -j DNAT --to-destination 10.0.1.2:80
iptables -t nat -A PREROUTING -i enp0s8 -p tcp --dport 443 -j DNAT --to-destination 10.0.1.2:443
iptables -t nat -A PREROUTING -i enp0s8 -p tcp --dport 22 -j DNAT --to-destination 10.0.3.2:22

echo "Creado."

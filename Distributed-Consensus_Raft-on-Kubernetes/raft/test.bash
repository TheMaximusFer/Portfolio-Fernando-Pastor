#!/bin/bash

# Configuración
CLIENTE="cltraft_go.yaml"
SERVIDOR="srvraft_go.yaml"
POD_CLI="clientraft"

#Función para ejecutar cliente y validar éxito.
verificar_cliente() {
    kubectl delete pod $POD_CLI --grace-period=0 --force 2>/dev/null
    kubectl apply -f $CLIENTE
    echo "   ...Esperando ejecución del cliente..."
    #El script se pausa aquí hasta que el pod acaba su trabajo (estado Succeeded o Failed).
    while true; do
        #Obtenemos el estado actual (Running, Pending, Succeeded, Failed...).
        PHASE=$(kubectl get pod $POD_CLI -o jsonpath='{.status.phase}' 2>/dev/null)
        
        # i ya acabó (bien o mal), salimos del bucle.
        if [[ "$PHASE" == "Succeeded" || "$PHASE" == "Failed" ]]; then
            break
        fi
        
        #Si sigue corriendo, esperamos un poco.
        sleep 2
    done

    LOGS=$(kubectl logs $POD_CLI)
    
    if echo "$LOGS" | grep -q "ÉXITO"; then
        echo "   -> OK: Prueba superada.\n"
        echo "---------------------------------"
        kubectl get pods -o wide
        echo "---------------------------------"
        echo "$LOGS"
        echo "---------------------------------"
    else
        echo "   -> ERROR: El cliente falló."
        echo "$LOGS"
        exit 1
    fi
}

echo "=== INICIO TEST RAFT EN KUBERNETES ==="

echo "[1] Desplegando Servidores..."
kubectl delete pod --all --grace-period=0 --force 2>/dev/null
kubectl apply -f $SERVIDOR
echo "   Esperando a que los pods estén preparados..."
sleep 5

echo "[2] Prueba Básica (Escritura/Lectura)"
verificar_cliente
# Guardamos logs para buscar al líder
LOGS_PREVIOS=$(kubectl logs $POD_CLI)
#Buscamos el id del lider a matar en los logs de la anterior ejecución.
LIDER_ID=$(echo "$LOGS_PREVIOS" | grep -o 'lider=[0-9]*' | tail -1 | cut -d= -f2)

echo "[3] Borrando Pod Líder (raft-$LIDER_ID)..."
kubectl delete pod raft-$LIDER_ID --grace-period=0 --force 2>/dev/null
echo "   Esperando regeneración..."
sleep 10
verificar_cliente # Comprobar que sigue funcionando
# Guardamos logs para buscar al líder
LOGS_PREVIOS=$(kubectl logs $POD_CLI)
#Buscamos el id del lider a matar en los logs de la anterior ejecución.
LIDER_ID=$(echo "$LOGS_PREVIOS" | grep -o 'lider=[0-9]*' | tail -1 | cut -d= -f2)
NODO_LIDER=$(kubectl get pods -o wide | grep -E "raft-$LIDER_ID.*" | awk '{print $7}' )

echo "[4] Parando contenedor Docker del nodo ($NODO_LIDER)..."
docker stop $NODO_LIDER
sleep 8
echo "   Probando con nodo caído..."
verificar_cliente

echo "[5] Restaurando nodo..."
docker start $NODO_LIDER
echo " "
kubectl get pods -o wide


echo "=== FIN ==="


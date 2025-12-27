/*****************
    CONSULTA 1
*****************/

--Calculamos la media de retrasos de cada compañía
CREATE MATERIALIZED VIEW MediaRetrasos AS
SELECT v.nombreCompania, AVG(r.duracion) AS media_retrasos
FROM Vuelo v
LEFT JOIN Retraso r ON v.id = r.id
WHERE v.cancelado = 0
GROUP BY v.nombreCompania;

--Número de vuelos de cada compañía cada día
WITH VuelosDiarios AS (
	SELECT nombreCompania, fechaSalida, COUNT(*) AS vuelos_diarios
	FROM Vuelo
	WHERE cancelado = 0
	GROUP BY nombreCompania, fechaSalida
),
--Seleccionamos de la tabla anterior aquellas compañias en las que
--todos los días tengan 1000 o más vuelos
CompaniasConMin1000Vuelos AS (
	SELECT nombreCompania
	FROM VuelosDiarios
	GROUP BY nombreCompania
	HAVING MIN(vuelos_diarios) >= 1000
)
SELECT m.nombreCompania, m.media_retrasos
FROM MediaRetrasos m
JOIN CompaniasConMin1000Vuelos c ON m.nombreCompania = c.nombreCompania
ORDER BY m.media_retrasos ASC;

/*****************
    CONSULTA 2
*****************/

-- 1. Encontrar la compañía con más aviones
WITH avionesPorCompania AS (
    SELECT nombreCompania, COUNT(*) AS cantidadAviones
    FROM Vuelo
    JOIN Avion ON Vuelo.matriculaAvion = Avion.numeroMatricula
    GROUP BY Vuelo.nombreCompania
),
CompaniaConMasAviones AS (
    SELECT avionesPorCompania.nombreCompania, avionesPorCompania.cantidadAviones 
    FROM avionesPorCompania
    WHERE avionesPorCompania.cantidadAviones = (SELECT MAX(cantidadAviones) FROM avionesPorCompania)
)
-- 2. Listar aeropuertos en Alaska o California donde la compañía no opera
SELECT A.nombreAeropuerto
FROM Aeropuerto A
WHERE (A.estado = 'AK' OR A.estado = 'CA') 
AND A.IATA NOT IN (
    SELECT V.origen
    FROM Vuelo V
    JOIN CompaniaConMasAviones C ON V.nombreCompania = C.nombreCompania
);

/*****************
    CONSULTA 3
*****************/

-- 1. Calcular las edades de los aviones de cada aeropuerto
WITH EdadesPorVuelo AS (
    SELECT a.IATA, a.nombreAeropuerto, 2025 - av.annoCompra AS Edad
    FROM Aeropuerto a
    JOIN Vuelo v ON (a.IATA = v.origen OR a.IATA = v.destino)
    JOIN Avion av ON v.matriculaAvion = av.numeroMatricula
    WHERE v.cancelado = 0
), -- 2. Calcular la media de edades de los aviones para cada aeropuerto
MediaPorAeropuerto AS (
    SELECT IATA, nombreAeropuerto, AVG(Edad) AS MediaEdad
    FROM EdadesPorVuelo
    GROUP BY IATA, nombreAeropuerto
)
-- 3. Proyectar el aeropuoerto con la media de edades de los aviones mas jovenes
SELECT IATA AS CodigoAeropuerto, nombreAeropuerto AS NombreAeropuerto, MediaEdad
FROM MediaPorAeropuerto
WHERE MediaEdad = (SELECT MIN(MediaEdad) FROM MediaPorAeropuerto);
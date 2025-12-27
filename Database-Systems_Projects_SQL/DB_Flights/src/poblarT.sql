
INSERT INTO Aeropuerto (IATA, pais,	ciudad,	nombreAeropuerto, estado)
SELECT DISTINCT originiata, origincountry, origincity, originairport, originstate FROM datosdb.datosvuelos
WHERE originiata is not NULL
UNION 
SELECT DISTINCT destiata, destcountry, destcity, destairport, deststate FROM datosdb.datosvuelos
WHERE destiata is not NULL;

INSERT INTO Compania (nombreCompania)
SELECT DISTINCT carrierName FROM datosdb.datosvuelos;

INSERT INTO Avion (numeroMatricula, fabricante, tipoMotor, annoCompra, modelo)
SELECT DISTINCT tailNum, planeManufacturer, plane_Engine_type, planeYear, planeModel FROM datosdb.datosvuelos
WHERE tailNum is not NULL;

INSERT INTO Vuelo (numeroVuelo, origen, destino, fechaSalida, nombreCompania, id, horaSalida, horaLLegada, matriculaAvion, cancelado)
WITH vuelosTemporales AS (
    SELECT DISTINCT flightNum, originiata, destiata, flightDate, carrierName FROM datosdb.datosvuelos
    WHERE flightNum IS NOT NULL AND originiata IS NOT NULL AND destiata IS NOT NULL AND carrierName IS NOT NULL AND flightDate IS NOT NULL
)
SELECT d.flightNum, d.originiata, d.destiata, d.flightDate, d.carrierName, idVuelo.NEXTVAL, d.crsDepTime, d.crsArrTime, d.tailNum, d.cancelled 
FROM datosdb.datosvuelos d
JOIN vuelosTemporales v
ON v.flightNum = d.flightNum
AND v.originiata = d.originiata
AND v.destiata = d.destiata
AND v.flightDate = d.flightDate
AND v.carrierName = d.carrierName;

-- INSERT INTO Vuelo (horaSalida, horaLLegada, matriculaAvion, cancelado)
-- SELECT DISTINCT crsDepTime, crsArrTime, tailNum, cancelled FROM datosdb.datosvuelos d
-- JOIN Vuelo v
-- ON v.numeroVuelo = d.flightNum
-- AND v.origen = d.originiata
-- AND v.destino = d.destiata
-- AND v.fechaSalida = d.flightDate
-- AND v.nombreCompania = d.carrierName;

INSERT INTO CambioDeRuta (id, IATAdesvio)
SELECT DISTINCT id, div1airport FROM (
    SELECT v.id, d.div1airport FROM datosdb.datosvuelos d
    JOIN Vuelo v
    ON v.numeroVuelo = d.flightNum
    AND v.origen = d.originiata
    AND v.destino = d.destiata
    AND v.fechaSalida = d.flightDate
    AND v.nombreCompania = d.carrierName
)
WHERE div1airport is not NULL;

INSERT INTO Retraso (id, causa, duracion)
WITH vuelosTemporales AS (
    SELECT DISTINCT flightNum, carrierDelay, weatherDelay, nasDelay, securityDelay, lateAircraftDelay, v.id FROM datosdb.datosvuelos d
    JOIN Vuelo v
    ON v.numeroVuelo = d.flightNum
    AND v.origen = d.originiata
    AND v.destino = d.destiata
    AND v.fechaSalida = d.flightDate
    AND v.nombreCompania = d.carrierName
)
SELECT DISTINCT v.id, 'Compania del vuelo', carrierDelay FROM vuelosTemporales v
WHERE v.carrierDelay > 0
UNION
SELECT DISTINCT v.id, 'Condiciones meteorologicas', weatherDelay FROM vuelosTemporales v
WHERE v.weatherDelay > 0
UNION
SELECT DISTINCT v.id, 'Centro de control (NAS)', nasDelay FROM vuelosTemporales v
WHERE v.nasDelay > 0
UNION
SELECT DISTINCT v.id, 'Intervencion de la seguridad del aeropuerto', securityDelay FROM vuelosTemporales v
WHERE v.securityDelay > 0
UNION
SELECT DISTINCT v.id, 'Llegada tardia del avion', lateAircraftDelay FROM vuelosTemporales v
WHERE v.lateAircraftDelay > 0;


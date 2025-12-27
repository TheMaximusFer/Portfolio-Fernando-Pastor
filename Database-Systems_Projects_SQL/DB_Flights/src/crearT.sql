/* Creacion de las tablas */
CREATE TABLE Aeropuerto (
	IATA				VARCHAR(7) PRIMARY KEY,
	pais				VARCHAR(30),
	estado				VARCHAR(30),
	ciudad				VARCHAR(30),
	nombreAeropuerto	VARCHAR(50)
)
PARTITION BY LIST (estado) (
    PARTITION p_alaska_california VALUES ('AK', 'CA'),
    PARTITION p_otros VALUES (DEFAULT)
);

CREATE TABLE Compania (
  	nombreCompania		VARCHAR(100) PRIMARY KEY
);

CREATE TABLE Avion (
	numeroMatricula		VARCHAR(30) PRIMARY KEY,
	fabricante			VARCHAR(30),
	tipoMotor			VARCHAR(30),
	annoCompra			NUMBER,
	modelo				VARCHAR(30)
);

CREATE SEQUENCE idVuelo START WITH 1 INCREMENT BY 1;

CREATE TABLE Vuelo (
	numeroVuelo			NUMBER,
	horaSalida			NUMBER,
	horaLLegada			NUMBER,
	fechaSalida			VARCHAR(10),
	nombreCompania		VARCHAR(100),
	origen				VARCHAR(7),
	destino				VARCHAR(7),
	matriculaAvion		VARCHAR(30),
	cancelado			NUMBER(1,0), -- Usado como booleano: 0 es false y 1 es true
	FOREIGN KEY (nombreCompania) REFERENCES Compania(nombreCompania),
	FOREIGN KEY (origen) REFERENCES Aeropuerto(IATA),
	FOREIGN KEY (destino) REFERENCES Aeropuerto(IATA),
	FOREIGN KEY (matriculaAvion) REFERENCES Avion(numeroMatricula),
	id				NUMBER PRIMARY KEY
);

CREATE TABLE CambioDeRuta (
	id					NUMBER PRIMARY KEY,
	IATAdesvio			VARCHAR(7),
	FOREIGN KEY (id) REFERENCES Vuelo(id)
);

CREATE TABLE Retraso (
	id					NUMBER,
	causa				VARCHAR(50),
	duracion			NUMBER,
	PRIMARY KEY (id, causa),
	FOREIGN KEY (id) REFERENCES Vuelo(id)
);

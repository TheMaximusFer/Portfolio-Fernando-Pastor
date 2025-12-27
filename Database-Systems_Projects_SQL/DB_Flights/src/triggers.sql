/*****************
     TRIGGER 1
*****************/
-- Trigger que detecta si intentas insertar un vuelo en el que el origen y el destino son el mismo
CREATE OR REPLACE TRIGGER mismoOrigenYDestino
BEFORE INSERT OR UPDATE ON Vuelo 
FOR EACH ROW
WHEN (NEW.origen = NEW.destino)
BEGIN
    RAISE_APPLICATION_ERROR(-20001, 'Un vuelo no puede tener origen y destino iguales.');
END;
/

/*****************
     TRIGGER 2
*****************/
-- Trigger que elimina todos los retrasos y cambios de ruta asociados a un vuelo al borrar dicho vuelo
CREATE OR REPLACE TRIGGER limpiarIncidenciasVuelo
BEFORE DELETE ON Vuelo
FOR EACH ROW
BEGIN
    DELETE FROM Retraso r WHERE r.id = :NEW.id;
    DELETE FROM CambioDeRuta cr WHERE cr.id = :NEW.id;
END;
/ 

/*****************
     TRIGGER 3
*****************/
-- Trigger que no permite actualizar un vuelo cancelado
CREATE OR REPLACE TRIGGER noActualizarVueloCancelado
BEFORE UPDATE ON Vuelo
FOR EACH ROW
WHEN (OLD.cancelado = 1)
BEGIN
    RAISE_APPLICATION_ERROR(-20002, 'No puedes actualizar un vuelo cancelado');
END;
/

CREATE OR REPLACE TRIGGER noInsertarRetrasoEnCancelado
BEFORE INSERT ON Retraso
FOR EACH ROW
DECLARE
    vueloExiste NUMBER;
    vueloCancelado NUMBER;
BEGIN
    SELECT COUNT(*) INTO vueloExiste FROM Vuelo WHERE id = :NEW.id;
    IF vueloExiste = 0 THEN 
        RAISE_APPLICATION_ERROR(-20003, 'No puedes insertar un retraso a un vuelo que no existe');
    ELSE
        SELECT cancelado INTO vueloCancelado FROM Vuelo WHERE id = :NEW.id;
        IF vueloCancelado = 1 THEN 
            RAISE_APPLICATION_ERROR(-20004, 'No puedes insertar un retraso a un vuelo cancelado');
        END IF;
    END IF;
END;
/

CREATE OR REPLACE TRIGGER noInsertarCambioDeRutaEnCancelado
BEFORE INSERT ON CambioDeRuta
FOR EACH ROW
DECLARE
    vueloExiste NUMBER;
    vueloCancelado NUMBER;
BEGIN
    SELECT COUNT(*) INTO vueloExiste FROM Vuelo WHERE id = :NEW.id;
    IF vueloExiste = 0 THEN 
        RAISE_APPLICATION_ERROR(-20003, 'No puedes insertar un cambio de ruta a un vuelo que no existe');
    ELSE
        SELECT cancelado INTO vueloCancelado FROM Vuelo WHERE id = :NEW.id;
        IF vueloCancelado = 1 THEN 
            RAISE_APPLICATION_ERROR(-20004, 'No puedes insertar un cambio de ruta a un vuelo cancelado');
        END IF;
    END IF;
END;
/


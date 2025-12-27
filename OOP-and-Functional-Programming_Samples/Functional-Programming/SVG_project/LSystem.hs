module LSystem where

import Turtle


-- Genera los puntos en funcion del comando
tplot :: Turtle -> String -> [Position]
tplot turtle [] = [obtenerPosicion turtle]
tplot turtle (simbolo:comando) = lineaDibujada ++ tplot nuevaTurtle comando
    where
        (nuevaTurtle, lineaDibujada) = mover turtle simbolo

-- Obtiene la posicion de la turtle
obtenerPosicion :: Turtle -> Position
obtenerPosicion (_, _, pos, _) = pos

-- Avanza la tortuga
avanzar :: Turtle -> (Turtle, [Position])
avanzar turtle = (nuevaTurtle, [antiguaPos, nuevaPos])
    where
        nuevaTurtle = moveTurtle turtle Forward
        antiguaPos = obtenerPosicion turtle
        nuevaPos = obtenerPosicion nuevaTurtle

-- mover la turtle una posición
mover :: Turtle -> Char -> (Turtle, [Position])
mover turtle simbolo =
    case simbolo of
        '>' -> avanzar turtle -- Avanzar
            
        '+' -> (moveTurtle turtle TurnRight, []) -- Girar derecha (sentido horario)
            
        '-' -> (moveTurtle turtle TurnLeft, []) -- Girar izquierda (sentido antihorario)

        a | a >= 'A' && a <= 'Z' -> avanzar turtle -- Avanzar

        a | a >= 'a' && a <= 'z' -> (turtle, []) -- Ignorar

        _                        -> (turtle, []) -- Ignorar


lsystem :: (Char -> String) -> String -> Int -> String
lsystem _ axioma 0 = axioma
lsystem rules axioma n = lsystem rules (concatMap sustituir axioma) (n - 1) --cocantMap es la composición entre concat y map
  where
    --sustituir coge un caracter, si al llamar a la función de rules se nos devuelve una cadena vacía, 
    --lo sustituimos por el mismo caracter, si no, lo sustituimos por la cadena que nos devuelve la función.
    sustituir x = (let r = rules x in 
                    if null r then [x] 
                    else r
                  )

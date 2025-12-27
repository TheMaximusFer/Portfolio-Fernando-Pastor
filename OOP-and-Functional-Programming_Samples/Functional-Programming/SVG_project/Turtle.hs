module Turtle where

-- Posicion en coordenadas (x,y)
type Position = (Double,Double)
-- Orientacion de la tortuga, se supone en grados
type Heading  = Double
-- Turtle = (paso,giro,posicion,orientacion)
type Turtle   = (Double,Double,Position,Heading)

data Move     = Forward | TurnLeft | TurnRight

-- Devuelve un nuevo estado de la tortuga,
-- según el movimiento deseado.
-- Los datos de paso y giro no cambian.
moveTurtle :: Turtle -> Move -> Turtle
moveTurtle (paso,giro,(x,y),orn) Forward = (paso,giro,(x+paso*c,y+paso*s),orn)
  where
    ornrad = (pi/180.0)*orn
    c = cos ornrad
    s = sin ornrad
moveTurtle (paso,giro,pos,orn) TurnLeft  = (paso,giro,pos,orn+giro)
moveTurtle (paso,giro,pos,orn) TurnRight = (paso,giro,pos,orn-giro)

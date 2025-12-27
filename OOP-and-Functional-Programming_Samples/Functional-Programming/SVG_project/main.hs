-- main.hs
module Main where

import LSystem
import SVG (savesvg)
import Turtle

--1. CURVA DE KOCH (ángulo 60º)
rulesKoch :: Char -> String
rulesKoch 'F' = "F-F++F-F"
rulesKoch  _  = ""

--2. CURVA DE KOCH CUADRADA (ángulo 90º)
rulesKochSquare :: Char -> String
rulesKochSquare 'F' = "F-F+F+F-F"
rulesKochSquare  _  = ""

--3. KOCH SNOWFLAKE (ángulo 60º)
rulesSnowflake :: Char -> String
rulesSnowflake 'F' = "F-F++F-F"
rulesSnowflake  _  = ""

--4. ISLA DE MINKOWSKI (ángulo 90º)
rulesMinkowski :: Char -> String
rulesMinkowski 'F' = "F-F+F+FF-F-F+F"
rulesMinkowski  _  = ""

--5. TRIÁNGULO DE SIERPINSKI (ángulo 120º)
rulesSierpTri :: Char -> String
rulesSierpTri 'F' = "F-G+F+G-F"
rulesSierpTri 'G' = "GG"
rulesSierpTri  _  = ""

--6. SIERPINSKI ARROWHEAD (ángulo 60º)
rulesArrowhead :: Char -> String
rulesArrowhead 'F' = "G+F+G"
rulesArrowhead 'G' = "F-G-F"
rulesArrowhead  _  = ""

--7. CURVA DE HILBERT (ángulo 90º)
rulesHilbert :: Char -> String
rulesHilbert 'f' = "-g>+f>f+>g-"
rulesHilbert 'g' = "+f>-g>g->f+"
rulesHilbert  _  = ""

-- 8. CURVA DE GOSPER (ángulo 60º)
rulesGosper :: Char -> String
rulesGosper 'F' = "F-G--G+F++FF+G-"
rulesGosper 'G' = "+F-GG--G-F++F+G"
rulesGosper  _  = ""

--             (nombre, rules, axioma, iter, turtle)
type Sistema = (String, Char -> String, String, Int, Turtle)

sistemas :: [Sistema]
sistemas =
  [ ("koch",        rulesKoch,        "F",          4, (1,60 ,(0,0),0))
  , ("kochSquare",  rulesKochSquare,  "F",          4, (1,90 ,(0,0),0))
  , ("snowflake",   rulesSnowflake,   "F++F++F",    4, (1,60 ,(0,0),0))
  , ("minkowski",   rulesMinkowski,   "F+F+F+F",    3, (1,90 ,(0,0),0))
  , ("sierpTri",    rulesSierpTri,    "F-G-G",      5, (1,120,(0,0),0))
  , ("arrowhead",   rulesArrowhead,   "F",          7, (1,60 ,(0,0),0))
  , ("hilbert",     rulesHilbert,     "f",          6, (1,90 ,(0,0),0))
  , ("gosper",      rulesGosper,      "F",          4, (1,60 ,(0,0),0))
  ]

--Generamos los SVG
main :: IO ()
main = do
  mapM_ genera sistemas
  putStrLn "SVG generados."
  where
    genera (nombre, rules, axioma, n, turtle) = do
      let comandos = lsystem rules axioma n
          puntos   = tplot turtle comandos
      savesvg nombre puntos

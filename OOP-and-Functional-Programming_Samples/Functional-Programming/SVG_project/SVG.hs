module SVG(savesvg) where

import Text.Printf

polyline :: [(Double, Double)] -> String
polyline vertices =
  "<svg xmlns=\"http://www.w3.org/2000/svg\"\n"
  ++ printf " viewBox=\"%f %f %f %f\"\n" bx by bw bh
  ++ printf " transform=\"scale(1 -1)\"\n"
  ++ ">\n"
  ++ "<polyline\n"
  ++ " fill=\"#F0FFF0\" stroke=\"black\" stroke-width=\"0.25%\"\n"
  ++ " points=\"\n"
  ++ (concat $ map (\(x,y) -> printf "%10.6f,%10.6f\n" x y) vertices)
  ++ " \"\n"
  ++ "/>\n"
  ++ "</svg>\n"
  where
    (minx,miny) = foldl1 (\(mx,my) (x,y) -> (min mx x,min my y)) vertices
    (maxx,maxy) = foldl1 (\(mx,my) (x,y) -> (max mx x,max my y)) vertices
    width  = maxx-minx
    height = maxy-miny
    margin = 0.05*max width height
    bx = minx-margin
    by = miny-margin
    bw = width+2*margin
    bh = height+2*margin

savesvg name pts = writeFile (name++".svg") (polyline pts)

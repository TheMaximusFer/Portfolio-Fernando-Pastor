module BinaryTree where

import Data.List (sort) 

-- Definición de BinaryTree
data BinaryTree a = Empty | Nodo a (BinaryTree a) (BinaryTree a) deriving (Eq)

-- Árbol vacío
empty :: BinaryTree a
empty = Empty

-- Crear una hoja
leaf :: a -> BinaryTree a
leaf x = Nodo x Empty Empty

-- Crear un árbol
tree :: a -> BinaryTree a -> BinaryTree a -> BinaryTree a
tree x ic rc = Nodo x ic rc

-- Calcula el tamaño del árbol
size :: BinaryTree a -> Int
size Empty = 0
size (Nodo _ izda dcha) = 1 + size izda + size dcha

-- Show para mostrar el árbol
instance (Show a, Eq a) => Show (BinaryTree a) where
  show t = unlines (dib t "" True True) --último boll indica que es raiz
    where
      dib :: BinaryTree a -> String -> Bool -> Bool -> [String]
      dib Empty _ _ _ = []
      dib (Nodo v izda dcha) pref esUlt esRaiz =
        let conector = if esRaiz then "" else if esUlt then "└─ " else "├─ "
            linea    = pref ++ conector ++ show v
            pref'    = pref ++ (if esRaiz then "" else if esUlt then "   " else "│  ")
            izdaTxt  = dib izda pref' (dcha == Empty) False
            dchaTxt  = dib dcha pref' True            False
        in  linea : izdaTxt ++ dchaTxt



-- Añade un elemento al árbol 
add :: (Ord a) => BinaryTree a -> a -> BinaryTree a
add Empty x = leaf x 
add (Nodo val izda dcha) x
    | x < val    = Nodo val (add izda x) dcha  
    | otherwise  = Nodo val izda (add dcha x)  

-- Construye un ABB
build :: (Ord a) => [a] -> BinaryTree a
build = foldl add Empty

buildBalanced :: Ord a => [a] -> BinaryTree a
buildBalanced [] = Empty
buildBalanced xs =
  let ys = sort xs                       --ordenamos la lista
      m  = ys !! (length ys `div` 2)     --elemento central
      left  = [x | x <- ys, x <  m]      --estrictamente menores
      right = tail [x | x <- ys, x >= m] --resto (quitamos la primera copia)
  in Nodo m (buildBalanced left) (buildBalanced right)

type DList a = [a] -> [a]
toD :: [a] -> DList a
toD xs = (xs ++)
fromD :: DList a -> [a]
fromD dl = dl []


preorder   :: BinaryTree a -> [a]
inorder    :: BinaryTree a -> [a]
postorder  :: BinaryTree a -> [a]

preorder  = fromD . pre   where
  pre Empty         = id
  pre (Nodo v l r)  = toD [v] . pre l . pre r

inorder = fromD . ino     where
  ino Empty         = id
  ino (Nodo v l r)  = ino l . toD [v] . ino r

postorder = fromD . pos   where
  pos Empty         = id
  pos (Nodo v l r)  = pos l . pos r . toD [v]


balance :: Ord a => BinaryTree a -> BinaryTree a
between :: Ord a => BinaryTree a -> a -> a -> [a]

balance = buildBalanced . inorder

between t lo hi = go t []
  where
    go Empty         acc = acc
    go (Nodo v l r)  acc
      | v < lo  = go r acc
      | v > hi  = go l acc
      | otherwise = v : go l (go r acc)

-- main :: IO ()
-- main = do
--     -- Construimos un ABB desde una lista desordenada
--     let numeros =  [3,3,3,3,3]
--     let arbol = build numeros
--     let arbolBalanced = buildBalanced numeros
    
--     putStrLn "Árbol Binario de Búsqueda construido:"
--     print arbolBalanced
    
--     putStrLn $"\nTamaño del árbol: " ++ show (size arbolBalanced)
    
--     -- Añadimos un nuevo elemento
--     let arbol2 = add arbol 1
--     putStrLn "\nÁrbol después de añadir 1:"
--     print arbol2

{-# LANGUAGE GADTs, TupleSections #-}
module Main where

import Data.Maybe (isJust, fromJust)
import Data.Bits ((.|.))
import Prelude hiding (not, and, or)

data Trit = TO | TI | TJ deriving (Show, Eq, Ord, Enum, Bounded)

tabs = [(ta,tb) | ta <- [TJ,TO,TI], tb <- [TJ,TO,TI]]

data Bit = O | I deriving (Show, Eq, Ord, Enum, Bounded)

not :: Bit -> Bit
not O = I
not I = O

xor :: Bit -> Bit -> Bit
xor O O = O
xor O I = I
xor I O = I
xor I I = O

and :: Bit -> Bit -> Bit
and O = const O
and I = id

or :: Bit -> Bit -> Bit
or O = id
or I = const I

andn x = and (not x)

orn x = or (not x)

tcurlS2 :: Trit -> Trit -> Trit
tcurlS2 a b = case (a,b) of
  (TJ,TJ) -> TI
  (TO,TJ) -> TO
  (TI,TJ) -> TJ
  (TJ,TO) -> TI
  (TO,TO) -> TJ
  (TI,TO) -> TO
  (TJ,TI) -> TJ
  (TO,TI) -> TI
  (TI,TI) -> TO

tcurlS2s = map (uncurry tcurlS2) tabs

data E where
  One :: E
  AL :: E
  AH :: E
  BL :: E
  BH :: E
  Not :: E -> E
  Xor :: E -> E -> E
  And :: E -> E -> E
  Or :: E -> E -> E
  Andn :: E -> E -> E
  Orn :: E -> E -> E
  -- Tern :: ? -> E -> E -> E -> E
  deriving (Show, Eq, Ord)

support :: E -> Int
support e = case e of
  One -> 0
  AL -> 1
  AH -> 2
  BL -> 4
  BH -> 8
  Not x -> support x
  Xor x y -> support x .|. support y
  And x y -> support x .|. support y
  Or x y -> support x .|. support y
  Andn x y -> support x .|. support y
  Orn x y -> support x .|. support y

isFullSupport :: E -> Bool
isFullSupport e = support e == 15

evalE :: (Bit,Bit) -> (Bit,Bit) -> E -> Bit
evalE (al,ah) (bl,bh) = f where
  f e = case e of
    One -> I
    AL -> al
    AH -> ah
    BL -> bl
    BH -> bh
    Not x -> not (f x)
    Xor x y -> xor (f x) (f y)
    And x y -> and (f x) (f y)
    Or x y -> or (f x) (f y)
    Andn x y -> andn (f x) (f y)
    Orn x y -> orn (f x) (f y)
    -- Tern

-- trit to ptrit map
newtype Cvt = Cvt [(Bit,Bit)] deriving (Eq, Ord)
instance Show Cvt where
  showsPrec p (Cvt [(ol,oh),(il,ih),(jl,jh)]) =
    showsPrec p jl . showsPrec p jh . (':':) .
    showsPrec p ol . showsPrec p oh . (':':) .
    showsPrec p il . showsPrec p ih

evalCvt :: Cvt -> Trit -> (Bit,Bit)
evalCvt (Cvt m) t = m !! fromEnum t

evalsECvt :: Cvt -> E -> [Bit]
evalsECvt cvt = f where
  abs = [(evalCvt cvt ta, evalCvt cvt tb) | (ta,tb) <- tabs]
  f e = case e of
    One -> map (const I) abs
    AL -> map (\((al,ah), (bl,bh)) -> al) abs
    AH -> map (\((al,ah), (bl,bh)) -> ah) abs
    BL -> map (\((al,ah), (bl,bh)) -> bl) abs
    BH -> map (\((al,ah), (bl,bh)) -> bh) abs
    Not x -> map not (f x)
    Xor x y -> zipWith xor (f x) (f y)
    And x y -> zipWith and (f x) (f y)
    Or x y -> zipWith or (f x) (f y)
    Andn x y -> zipWith andn (f x) (f y)
    Orn x y -> zipWith orn (f x) (f y)
    -- Tern

isCurlS2 :: Cvt -> Cvt -> (E,E) -> Bool
isCurlS2 cvtOut cvtIn (cl,ch) = lhs == rhs where
  lhs = zip (evalsECvt cvtIn cl) (evalsECvt cvtIn ch)
  rhs = map (evalCvt cvtOut) tcurlS2s

isCurlS2' :: ([Bit],[Bit]) -> Cvt -> [E] -> Maybe (E,E)
isCurlS2' (cls,chs) cvtIn cs = do
  let css = map (\c -> (evalsECvt cvtIn c, c)) cs
  ecl <- lookup cls css
  ech <- lookup chs css
  return (ecl,ech)

findCurlS2 :: Cvt -> Cvt -> [[E]] -> [(E,E)]
findCurlS2 cvtOut cvtIn es = result where
  cls'chs = unzip $ map (evalCvt cvtOut) tcurlS2s
  result = [fromJust mee | cs <- es, let mee = isCurlS2' cls'chs cvtIn cs, isJust mee]

select3of4 :: [a] -> [[a]]
select3of4 [a,b,c,d] = [[a,b,c],[a,b,d],[a,c,d],[b,c,d]]

perm3 :: [a] -> [[a]]
perm3 [a,b,c] = [[a,b,c],[a,c,b],[b,c,a],[b,a,c],[c,a,b],[c,b,a]]

cvts :: [Cvt]
cvts = map Cvt . concatMap perm3 . select3of4 $ [(O,O),(O,I),(I,O),(I,I)]

ecurlS2 :: (E,E)
ecurlS2 = (cl,ch) where
  delta = And AL (Xor BL AH)
  cl = Not delta
  ch = Or (Xor AL BH) delta

ptritCvt :: Cvt
ptritCvt = Cvt $ map f [minBound .. maxBound] where
  f TO = (I,I)
  f TI = (O,I)
  f TJ = (I,O)

testPtritCurl = isCurlS2 ptritCvt ptritCvt ecurlS2

select2s :: [a] -> [(a,a)]
select2s [] = []
select2s (x:xs) = map (x,) xs ++ select2s xs 

circuit :: Char -> [[E]]
circuit q = do
  let
    op0 = [AL,AH,BL,BH]
    op1 x = [Not x]
    op2 x y = [Xor x y, And x y, Or x y] -- ++ [Andn x y, Andn y x, Orn x y, Orn y x]
    -- op3 = [Tern ?]

    -- construct new expressions using unary op
    op1s :: [E] -> [E]
    op1s xs = map Not xs

    -- construct new expressions using binary op
    op2s :: [E] -> [E]
    -- there is no sense to apply a binary operation to `x`:
    -- `Xor x x = 0`, `Andn x x = 0`, `Or x x = x`
    -- `x` is used in `op2 x` and not used in `op2s xs`
    -- so there are no redundancies in the new list
    op2s = concatMap (uncurry op2) . select2s
      -- \xs -> concatMap (op2 x) xs ++ op2s xs

    -- construct new Es using binary op, `xs` and `ys` must not intersect
    allOp2s xs ys = [r | x <- xs, y <- ys, r <- op2 x y]

    l0 = op0
    -- just one op applied to var/vars
    appOp args vars = op1s vars ++ op2s vars ++ [r | arg <- args, var <- vars, r <- op2 arg var]

  let
    aa = do
      -- x0 = al, ah, bl, bh
      x1 <- op1s l0 ++ op2s l0 -- not x0, x0 `op` y0
      x2 <- op1s [x1] ++ allOp2s l0 [x1] -- not x1, x0 `op` x1
      x3 <- op1s [x2] ++ allOp2s (x1:l0) [x2] -- not x2, x1 `op` x2
      x4 <- op1s [x3] ++ allOp2s (x2:x1:l0) [x3]
      x5 <- op1s [x4] ++ allOp2s (x3:x2:x1:l0) [x4]
      return [x1,x2,x3,x4,x5]

    bb = do
      -- x0 = al, ah, bl, bh
      (x1,x2) <- select2s $ op1s l0 ++ op2s l0 -- not x0, x0 `op` y0
      x3 <- op1s [x1,x2] ++ allOp2s l0 [x1,x2] -- not x2, x1 `op` x2
      x4 <- op1s [x3] ++ allOp2s (x2:x1:l0) [x3]
      x5 <- op1s [x4] ++ allOp2s (x3:x2:x1:l0) [x4]
      return [x1,x2,x3,x4,x5]

    cc = do
      -- x0 = al, ah, bl, bh
      x1 <- op1s l0 ++ op2s l0 -- not x0, x0 `op` y0
      (x2,x3) <- select2s $ op1s [x1] ++ allOp2s l0 [x1] -- not x1, x0 `op` x1
      x4 <- op1s [x2,x3] ++ allOp2s (x1:l0) [x2,x3]
      x5 <- op1s [x4] ++ allOp2s (x3:x2:x1:l0) [x4]
      return [x1,x2,x3,x4,x5]

    dd = do
      -- x0 = al, ah, bl, bh
      x1 <- op1s l0 ++ op2s l0 -- not x0, x0 `op` y0
      x2 <- op1s [x1] ++ allOp2s l0 [x1] -- not x1, x0 `op` x1
      (x3,x4) <- select2s $ op1s [x2] ++ allOp2s (x1:l0) [x2] -- not x2, x1 `op` x2
      x5 <- op1s [x3,x4] ++ allOp2s (x2:x1:l0) [x3,x4]
      return [x1,x2,x3,x4,x5]

    ee = do
      -- x0 = al, ah, bl, bh
      x1 <- op1s l0 ++ op2s l0 -- not x0, x0 `op` y0
      x2 <- op1s [x1] ++ allOp2s l0 [x1] -- not x1, x0 `op` x1
      x3 <- op1s [x2] ++ allOp2s (x1:l0) [x2] -- not x2, x1 `op` x2
      (x4,x5) <- select2s $ op1s [x3] ++ allOp2s (x2:x1:l0) [x3]
      return [x1,x2,x3,x4,x5]

  filter (isFullSupport . last) $ case q of
    'a' -> aa
    'b' -> bb
    'c' -> cc
    'd' -> dd
    'e' -> ee
    _ -> aa ++ bb ++ cc ++ dd ++ ee

st = findCurlS2 ptritCvt ptritCvt . circuit

tt = circuit
ltt = length . tt

circuit' :: [[E]]
circuit' = [l0,l1,l2,l3,l4,l5] where
  op0 = [AL,AH,BL,BH]
  op1 = [Not]
  op2 = [Xor, And, Or] -- [Xor, And, Or, Andn, Orn]
  -- op3 = [Tern ?]

  l0 = op0
  l1 = (op1 <*> l0) ++ (op2 <*> l0 <*> l0)
  l2 = (op1 <*> l1) ++ (op2 <*> l1 <*> l0) ++ (op2 <*> l0 <*> l1)
  l3 = (op1 <*> l2) ++ (op2 <*> l2 <*> l0) ++ (op2 <*> l1 <*> l1) ++ (op2 <*> l0 <*> l2)
  l4 = (op1 <*> l3) ++ (op2 <*> l3 <*> l0) ++ (op2 <*> l2 <*> l1) ++ (op2 <*> l1 <*> l2) ++ (op2 <*> l0 <*> l3)
  l5 = (op1 <*> l4) ++ (op2 <*> l4 <*> l0) ++ (op2 <*> l3 <*> l1) ++ (op2 <*> l2 <*> l2) ++ (op2 <*> l1 <*> l3) ++ (op2 <*> l0 <*> l4)
  ls = l0 ++ l1 ++ l2 ++ l3 ++ l4 -- ++ l5

main :: IO ()
main = mapM_ (putStrLn . show) (st ' ')

-- (Xor AH (And AL (Not BL)),Xor AL (And BH (Xor AH (And AL (Not BL)))))

-----------------------------------------------------------------------------
-- |
-- Module      :  Plugins.Monitors.Mem
-- Copyright   :  (c) Andrea Rossato
-- License     :  BSD-style (see LICENSE)
--
-- Maintainer  :  Jose A. Ortega Ruiz <jao@gnu.org>
-- Stability   :  unstable
-- Portability :  unportable
--
-- A memory monitor for Xmobar
--
-----------------------------------------------------------------------------

module Plugins.Monitors.Mem (memConfig, runMem, totalMem, usedMem) where

import Plugins.Monitors.Common

memConfig :: IO MConfig
memConfig = mkMConfig
       "Mem: <usedratio>% (<cache>M)" -- template
       ["usedbar", "freebar", "usedratio", "total",
        "free", "buffer", "cache", "rest", "used"]  -- available replacements

fileMEM :: IO String
fileMEM = readFile "/proc/meminfo"

parseMEM :: IO [Float]
parseMEM =
    do file <- fileMEM
       let content = map words $ take 4 $ lines file
           [total, free, buffer, cache] = map (\line -> (read $ line !! 1 :: Float) / 1024) content
           rest = free + buffer + cache
           used = total - rest
           usedratio = used / total
       return [usedratio, total, free, buffer, cache, rest, used]

totalMem :: IO Float
totalMem = fmap ((*1024) . (!!1)) parseMEM

usedMem :: IO Float
usedMem = fmap ((*1024) . (!!6)) parseMEM

formatMem :: [Float] -> Monitor [String]
formatMem (r:xs) =
    do let f = showDigits 0
           rr = 100 * r
       ub <- showPercentBar rr r
       fb <- showPercentBar (100 - rr) (1 - r)
       rs <- showPercentWithColors r
       s <- mapM (showWithColors f) xs
       return (ub:fb:rs:s)
formatMem _ = return $ replicate 9 "N/A"

runMem :: [String] -> Monitor String
runMem _ =
    do m <- io parseMEM
       l <- formatMem m
       parseTemplate l

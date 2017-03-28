------------------------------------------------------------------------------
-- |
-- Module      : Plugins.Monitors.Uptime
-- Copyright   : (c) 2010 Jose Antonio Ortega Ruiz
-- License     : BSD3-style (see LICENSE)
--
-- Maintainer  : jao@gnu.org
-- Stability   : unstable
-- Portability : unportable
-- Created: Sun Dec 12, 2010 20:26
--
--
-- Uptime
--
------------------------------------------------------------------------------


module Plugins.Monitors.Uptime (uptimeConfig, runUptime) where

import Plugins.Monitors.Common

import qualified Data.ByteString.Lazy.Char8 as B

uptimeConfig :: IO MConfig
uptimeConfig = mkMConfig "Up <days>d <hours>h <minutes>m"
                         ["days", "hours", "minutes", "seconds"]

readUptime :: IO Float
readUptime =
  fmap (read . B.unpack . head . B.words) (B.readFile "/proc/uptime")

secsPerDay :: Integer
secsPerDay = 24 * 3600

uptime :: Monitor [String]
uptime = do
  t <- io readUptime
  u <- getConfigValue useSuffix
  let tsecs = floor t
      secs = tsecs `mod` secsPerDay
      days = tsecs `quot` secsPerDay
      hours = secs `quot` 3600
      mins = (secs `mod` 3600) `div` 60
      ss = secs `mod` 60
      str x s = if u then show x ++ s else show x
  mapM (`showWithColors'` days)
       [str days "d", str hours "h", str mins "m", str ss "s"]

runUptime :: [String] -> Monitor String
runUptime _ = uptime >>= parseTemplate

------------------------------------------------------------------------------
-- |
-- Module       :  Plugins.Monitors.ThermalZone
-- Copyright    :  (c) 2011 Jose Antonio Ortega Ruiz
-- License      :  BSD3-style (see LICENSE)
--
-- Maintainer   :  jao@gnu.org
-- Stability    :  unstable
-- Portability  :  portable
-- Created      :  Fri Feb 25, 2011 03:18
--
--
-- A thermal zone plugin based on the sysfs linux interface.
-- See http://kernel.org/doc/Documentation/thermal/sysfs-api.txt
--
------------------------------------------------------------------------------

module Plugins.Monitors.ThermalZone (thermalZoneConfig, runThermalZone) where

import Plugins.Monitors.Common

import System.Posix.Files (fileExist)
import qualified Data.ByteString.Char8 as B

-- | Default thermal configuration.
thermalZoneConfig :: IO MConfig
thermalZoneConfig = mkMConfig "<temp>C" ["temp"]

-- | Retrieves thermal information. Argument is name of thermal
-- directory in \/sys\/clas\/thermal. Returns the monitor string
-- parsed according to template (either default or user specified).
runThermalZone :: [String] -> Monitor String
runThermalZone args = do
    let zone = head args
        file = "/sys/class/thermal/thermal_zone" ++ zone ++ "/temp"
        parse = return . (read :: String -> Int) . B.unpack
    exists <- io $ fileExist file
    if exists
      then do mdegrees <- io $ B.readFile file >>= parse
              temp <- showWithColors show (mdegrees `quot` 1000)
              parseTemplate [ temp ]
      else return "N/A"


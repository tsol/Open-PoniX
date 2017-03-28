-----------------------------------------------------------------------------
-- |
-- Module      :  Plugins.Monitors.Thermal
-- Copyright   :  (c) Juraj Hercek
-- License     :  BSD-style (see LICENSE)
--
-- Maintainer  :  Juraj Hercek <juhe_haskell@hck.sk>
-- Stability   :  unstable
-- Portability :  unportable
--
-- A thermal monitor for Xmobar
--
-----------------------------------------------------------------------------

module Plugins.Monitors.Thermal where

import qualified Data.ByteString.Lazy.Char8 as B
import Plugins.Monitors.Common
import System.Posix.Files (fileExist)

-- | Default thermal configuration.
thermalConfig :: IO MConfig
thermalConfig = mkMConfig
       "Thm: <temp>C" -- template
       ["temp"]       -- available replacements

-- | Retrieves thermal information. Argument is name of thermal directory in
-- \/proc\/acpi\/thermal_zone. Returns the monitor string parsed according to
-- template (either default or user specified).
runThermal :: [String] -> Monitor String
runThermal args = do
    let zone = head args
        file = "/proc/acpi/thermal_zone/" ++ zone ++ "/temperature"
    exists <- io $ fileExist file
    case exists of
         False  -> return $ "Thermal (" ++ zone ++ "): N/A"
         True   -> do number <- io $ B.readFile file
                                     >>= return . (read :: String -> Int)
                                                . stringParser (1, 0)
                      thermal <- showWithColors show number
                      parseTemplate [  thermal ]


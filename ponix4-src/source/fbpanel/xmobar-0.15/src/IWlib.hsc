-----------------------------------------------------------------------------
-- |
-- Module      :  IWlib
-- Copyright   :  (c) Jose A Ortega Ruiz
-- License     :  BSD-style (see LICENSE)
--
-- Maintainer  :  Jose A Ortega Ruiz <jao@gnu.org>
-- Stability   :  unstable
-- Portability :  unportable
--
--  A partial binding to iwlib
--
-----------------------------------------------------------------------------

{-# LANGUAGE CPP, ForeignFunctionInterface, EmptyDataDecls #-}


module IWlib (WirelessInfo(..), getWirelessInfo) where

import Foreign
import Foreign.C.Types
import Foreign.C.String

data WirelessInfo = WirelessInfo { wiEssid :: String,  wiQuality :: Int }
                  deriving Show

#include <iwlib.h>

data WCfg
data WStats
data WRange

foreign import ccall "iwlib.h iw_sockets_open"
  c_iw_open :: IO CInt

foreign import ccall "unistd.h close"
  c_iw_close :: CInt -> IO ()

foreign import ccall "iwlib.h iw_get_basic_config"
  c_iw_basic_config :: CInt -> CString -> Ptr WCfg -> IO CInt

foreign import ccall "iwlib.h iw_get_stats"
  c_iw_stats :: CInt -> CString -> Ptr WStats -> Ptr WRange -> CInt -> IO CInt

foreign import ccall "iwlib.h iw_get_range_info"
  c_iw_range :: CInt -> CString -> Ptr WRange -> IO CInt

getWirelessInfo :: String -> IO WirelessInfo
getWirelessInfo iface =
  allocaBytes (#size struct wireless_config) $ \wc ->
  allocaBytes (#size struct iw_statistics) $ \stats ->
  allocaBytes (#size struct iw_range) $ \rng ->
  withCString iface $ \istr -> do
    i <- c_iw_open
    bcr <- c_iw_basic_config i istr wc
    str <- c_iw_stats i istr stats rng 1
    rgr <- c_iw_range i istr rng
    c_iw_close i
    if (bcr < 0) then return WirelessInfo { wiEssid = "", wiQuality = 0 } else
      do hase <- (#peek struct wireless_config, has_essid) wc :: IO CInt
         eon <- (#peek struct wireless_config, essid_on) wc :: IO CInt
         essid <- if hase /= 0 && eon /= 0 then
                    do let e = (#ptr struct wireless_config, essid) wc
                       peekCString e
                  else return ""
         q <- if str >= 0 && rgr >=0 then
                do qualv <- xqual $ (#ptr struct iw_statistics, qual) stats
                   mv <- xqual $ (#ptr struct iw_range, max_qual) rng
                   let mxv = if mv /= 0 then fromIntegral mv else 1
                   return $ fromIntegral qualv / mxv
              else return 0
         let qv = round (100 * (q :: Double))
         return $ WirelessInfo { wiEssid = essid, wiQuality = min 100 qv }
    where xqual p = let qp = (#ptr struct iw_param, value) p in
            (#peek struct iw_quality, qual) qp :: IO CChar

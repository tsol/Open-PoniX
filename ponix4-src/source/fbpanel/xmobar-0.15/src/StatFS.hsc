-----------------------------------------------------------------------------
-- |
-- Module      :  StatFS
-- Copyright   :  (c) Jose A Ortega Ruiz
-- License     :  BSD-style (see LICENSE)
--
-- Maintainer  :  Jose A Ortega Ruiz <jao@gnu.org>
-- Stability   :  unstable
-- Portability :  unportable
--
--  A binding to C's statvfs(2)
--
-----------------------------------------------------------------------------

{-# LANGUAGE CPP, ForeignFunctionInterface, EmptyDataDecls #-}


module StatFS ( FileSystemStats(..), getFileSystemStats ) where

import Foreign
import Foreign.C.Types
import Foreign.C.String
import Data.ByteString (useAsCString)
import Data.ByteString.Char8 (pack)

#if  defined (__FreeBSD__) || defined (__OpenBSD__) ||  defined (__APPLE__) || defined (__DragonFly__)
#define IS_BSD_SYSTEM
#endif

#ifdef IS_BSD_SYSTEM
# include <sys/param.h>
# include <sys/mount.h>
#else
# include <sys/vfs.h>
#endif

data FileSystemStats = FileSystemStats {
  fsStatBlockSize :: Integer
  -- ^ Optimal transfer block size.
  , fsStatBlockCount :: Integer
  -- ^ Total data blocks in file system.
  , fsStatByteCount :: Integer
  -- ^ Total bytes in file system.
  , fsStatBytesFree :: Integer
  -- ^ Free bytes in file system.
  , fsStatBytesAvailable :: Integer
  -- ^ Free bytes available to non-superusers.
  , fsStatBytesUsed :: Integer
  -- ^ Bytes used.
  } deriving (Show, Eq)

data CStatfs

#ifdef IS_BSD_SYSTEM
foreign import ccall unsafe "sys/mount.h statfs"
#else
foreign import ccall unsafe "sys/vfs.h statfs64"
#endif
  c_statfs :: CString -> Ptr CStatfs -> IO CInt

toI :: CULong -> Integer
toI = toInteger

getFileSystemStats :: String -> IO (Maybe FileSystemStats)
getFileSystemStats path =
  allocaBytes (#size struct statfs) $ \vfs ->
  useAsCString (pack path) $ \cpath -> do
    res <- c_statfs cpath vfs
    if res == -1 then return Nothing
      else do
        bsize <- (#peek struct statfs, f_bsize) vfs
        bcount <- (#peek struct statfs, f_blocks) vfs
        bfree <- (#peek struct statfs, f_bfree) vfs
        bavail <- (#peek struct statfs, f_bavail) vfs
        let bpb = toI bsize
        return $ Just FileSystemStats
                       { fsStatBlockSize = bpb
                       , fsStatBlockCount = toI bcount
                       , fsStatByteCount = toI bcount * bpb
                       , fsStatBytesFree = toI bfree * bpb
                       , fsStatBytesAvailable = toI bavail * bpb
                       , fsStatBytesUsed = toI (bcount - bavail) * bpb
                       }

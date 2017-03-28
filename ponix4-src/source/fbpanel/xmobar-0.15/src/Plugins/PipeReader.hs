-----------------------------------------------------------------------------
-- |
-- Module      :  Plugins.PipeReader
-- Copyright   :  (c) Andrea Rossato
-- License     :  BSD-style (see LICENSE)
--
-- Maintainer  :  Jose A. Ortega Ruiz <jao@gnu.org>
-- Stability   :  unstable
-- Portability :  unportable
--
-- A plugin for reading from named pipes
--
-----------------------------------------------------------------------------

module Plugins.PipeReader where

import System.IO
import Plugins

data PipeReader = PipeReader String String
    deriving (Read, Show)

instance Exec PipeReader where
    alias (PipeReader _ a)    = a
    start (PipeReader p _) cb = do
        h <- openFile p ReadWriteMode
        forever (hGetLineSafe h >>= cb)
        where forever a = a >> forever a

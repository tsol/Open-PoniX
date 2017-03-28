-----------------------------------------------------------------------------
-- |
-- Module      :  Plugins.CommandReader
-- Copyright   :  (c) John Goerzen
-- License     :  BSD-style (see LICENSE)
--
-- Maintainer  :  Jose A. Ortega Ruiz <jao@gnu.org>
-- Stability   :  unstable
-- Portability :  unportable
--
-- A plugin for reading from external commands
-- note: stderr is lost here
--
-----------------------------------------------------------------------------

module Plugins.CommandReader where

import System.IO
import Plugins
import System.Process(runInteractiveCommand, getProcessExitCode)

data CommandReader = CommandReader String String
    deriving (Read, Show)

instance Exec CommandReader where
    alias (CommandReader _ a)    = a
    start (CommandReader p _) cb = do
        (hstdin, hstdout, hstderr, ph) <- runInteractiveCommand p
        hClose hstdin
        hClose hstderr
        hSetBinaryMode hstdout False
        hSetBuffering hstdout LineBuffering
        forever ph (hGetLineSafe hstdout >>= cb)
        where forever ph a =
                  do a
                     ec <- getProcessExitCode ph
                     case ec of
                       Nothing -> forever ph a
                       Just _ -> cb "EXITED"

-----------------------------------------------------------------------------
-- |
-- Module      :  Plugins.HelloWorld
-- Copyright   :  (c) Andrea Rossato
-- License     :  BSD-style (see LICENSE)
--
-- Maintainer  :  Jose A. Ortega Ruiz <jao@gnu.org>
-- Stability   :  unstable
-- Portability :  unportable
--
-- A plugin example for Xmobar, a text based status bar
--
-----------------------------------------------------------------------------

module Plugins.HelloWorld where

import Plugins

data HelloWorld = HelloWorld
    deriving (Read, Show)

instance Exec HelloWorld where
    alias HelloWorld = "helloWorld"
    run   HelloWorld = return "<fc=red>Hello World!!</fc>"

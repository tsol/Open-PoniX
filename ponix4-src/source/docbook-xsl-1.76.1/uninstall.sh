#!/bin/bash
mydir=$(cd -P $(dirname $0) && pwd -P)
$mydir/install.sh \
  --uninstall \
  --catalogManager=/root/.resolver/CatalogManager.properties \
  --dotEmacs='' \
  $@

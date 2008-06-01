#!/bin/sh

cd ../gfx
make
cd -

bldmake bldfiles
abld build gcce urel

cd ../sis
makesis barbelo.pkg barbelo.sis
signsis barbelo.sis barbelo.sisx mycert.cer mykey.key

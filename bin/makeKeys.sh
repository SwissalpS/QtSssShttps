#!/bin/sh
###
## makeKeys.sh
##
## create new self signed keys
##
## Copyleft 2019 Luke J. Zimmermann aka SwissalpS
###

## make private key with password
echo "remember the password you enter here!";
openssl genrsa -des3 -out sslKey.pp 4096;

## make signing request
openssl req -new -key sslKey.pp -out ssl.csr;

## self-sign the key for a year
openssl x509 -req -days 365 -in ssl.csr -signkey sslKey.pp -out ssl.crt;

## make a copy of private key without password
openssl rsa -in sslKey.pp -out sslKey;

exit 0;

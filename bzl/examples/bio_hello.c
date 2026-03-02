/**
   @file      bio_hello.c
   @author    Mitch Richling <https://www.mitchr.me/>
   @Copyright Copyright 2008 by Mitch Richling.  All rights reserved.
   @brief     Simple OpenSSL I/O example program.@EOL
   @Keywords  hello world openssl bio
   @Std       C99
              This code has no error checking so that the basic
              concepts are more visible.              
*/

#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>

int main(int argc, char *argv[]);

int main(int argc, char *argv[]) {

  BIO *bio_stdout;

  bio_stdout = BIO_new_fp(stdout, BIO_NOCLOSE);

  BIO_printf(bio_stdout, "hello, World!\n");

  BIO_free_all(bio_stdout);  

  return 0;
} /* end func main */
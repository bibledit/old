For adding secure transport to Bibledit, a number of SSL/TLS were considered.

OpenSSL is widely used but for embedded applications it may be a bit bloated.

GnuTLS was als considered but not further looked into.

WolfSSL
https://www.wolfssl.com
https://github.com/wolfSSL/wolfssl-examples

Botan
http://botan.randombit.net/
It can generate a single amalgamation file:
./configure.py --gen-amalgamation --single-amalgamation-file
There's a lot of dependency on the processor architecture, including endian-ness and assembler code.
And this means that every CPU needs to be configured separately.

PolarSSL, now mbed TLS
https://tls.mbed.org/
This looks good so far, it is low level, loosely coupled, easy to understand.

MatrixSSL
http://www.matrixssl.org/
The code looks okay so far, but the implementation in client is complicated, judging from the sample client application.

NanoSSL
https://www.mocana.com/iot-security/nanossl
This is proprietary code and cannot be downloaded and used freely.

After looking at all of them, the winner is: mbed TLS :)

Download the source tarball and unpack it.

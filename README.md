#OpenAAA
Distributed, Authentication, Authorization and Accouting Library

##Features

* TLS side channel authentication and straightforward bindings of AAA information to application layer using well-defined mechanism.
* Strong authentication without client certificates and/or passwords based on existing standards.
* Scalable, High Available, High Performance Session Management used for authorization and accounting authenticated users.

##Supports
OpenSSL, NSS (Network Security Services), PolarSSL (experimental), GnuTLS (experimental)

##Modules
OpenVPN [plugin], Apache2 [module], Firefox [pkcs11 module]

##Requires

Dynamically linked crypto libraries

##Specifications

#### OpenAAA
##### TLS side channel authentication and straightforward bindings of AAA information to application layer using well-defined mechanism.
https://github.com/n13l/openaaa/blob/master/doc/tls

#### OpenVPN
##### Added support for TLS Keying Material Exporters [RFC-5705]
https://github.com/n13l/openaaa/blob/master/doc/openvpn/keying-material-exporter

#!/bin/sh
openssl s_server -cert ./server.pem -key ./server.pem -accept 4443  -nextprotoneg alucid://localhost:1234/auth -auth -keymatexport EXPERIMENTAL

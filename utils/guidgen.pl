#!/usr/bin/perl
$uuid = `uuidgen`;
chomp $uuid;
print $uuid, "\n";
$parts = ($uuid =~ /^(.{8})-(.{4})-(.{4})-(..)(..)-(..)(..)(..)(..)(..)(..)$/);
print "{ 0x$parts[0], 0x$parts[1], 0x$parts[2], \\", "\n", " { ";
for (3 .. 9) {
print "0x$parts[$_], ";
}
print "0x$parts[10] } }", "\n";

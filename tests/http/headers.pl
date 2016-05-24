#!/usr/bin/perl --
use strict;
use warnings;
use CGI;

my $q = CGI->new;
my %headers = map { $_ => $q->http($_) } $q->http();

print $q->header('text/plain');
print "HTTP HEADERS:\n";
for my $header ( keys %headers ) {
	print "$header: $headers{$header}\n";
}

print "HTTP ENVS:\n";
foreach my $key (sort keys(%ENV)) {
	print "$key = $ENV{$key}\n";
}

#!/usr/bin/perl -w
# mk-tab.pl --- master loop to call the tests for accuracy
# Author: Alexander Weidauer <alex.weidauer@huckfinn.de>
# Created: 22 Mar 2016
# Version: 0.01
# --------------------------------------------------------
use warnings;
use strict;
# Read CMD Param
my $FROM =   shift;
my $TO   =   shift;
my $BY   =   shift;

# Set defaults if not found
$FROM = 1    if ! $FROM;
$TO   = 10   if ! $TO;
$BY   = 1000 if ! $BY;

# Calculate the table
print "NUM MX SX MY SY MN SD\n";
for my $i ($FROM..$TO) {
    my $num = $i*$BY;
    my $res=`./test-repro.R $num | grep STAT:`;
    chomp($res);
    $res =~ s/\s+/ /g; 
    $res =~ s/STAT: //g; 
    print $res,"\n";
}

__END__

=head1 NAME

mk-tab.pl - Creates an accuracy table for the reprojection test.

=head1 SYNOPSIS

mk-tab.pl from to by

mk-tab 1 10 2000

=head1 DESCRIPTION

Creates an accuracy table for the reprojection test.

=head1 AUTHOR

Alexander Weidauer, E<lt>iaw@igor.bigopensky.deE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2017 by Alexander Weidauer

This program is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.2 or,
at your option, any later version of Perl 5 you may have available.

=cut

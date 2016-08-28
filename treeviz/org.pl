#!/usr/bin/perl -w

open(CSVFILE, "data.csv") || die "Failed to open.";

$header = <CSVFILE>;
while (<CSVFILE>) {
	chomp;
	($date, $time, $temp, $odo) = $_ =~ /(\d\d\/\d\d\/\d\d) (\d\d:\d\d:\d\d),([0-9.]+),[0-9.]+,[0-9.]+,[0-9.]+,[0-9.]+,([0-9.]+)/;

	print "$temp $odo\n";
}

exit 0

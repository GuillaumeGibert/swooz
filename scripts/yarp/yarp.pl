

use strict;
use warnings;
use Cwd;

my $CurrentDirectory = cwd();

our $PBase         = $CurrentDirectory . "/../../";
our $PThird_party  = $PBase . "swooz-3rdparty/";


my $ARGS_YARP = "";
my $ii = 0;

while($ii < @ARGV)
{
    $ARGS_YARP = $ARGS_YARP . " " . $ARGV[$ii];
    $ii += 1;
}

$ENV{PATH} ="";

$ENV{PATH} = $PThird_party . "ace/ace-6.1.0/bin;" . $ENV{PATH};

chdir "bin";
system("yarp.exe " . $ARGS_YARP);
chdir "..";

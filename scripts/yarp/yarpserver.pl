

use strict;
use warnings;
use Cwd;

my $CurrentDirectory = cwd();

our $PBase         = $CurrentDirectory . "/../../";
our $PThird_party  = $PBase . "swooz-3rdparty/";

$ENV{PATH} ="";
$ENV{YARP_DATA_DIRS} ="";
$ENV{YARP_DIR} ="";
$ENV{PATH} = $PThird_party . "ace/ace-6.1.0/bin;" . $ENV{PATH};

chdir "bin";
system("yarpserver.exe");
chdir "..";

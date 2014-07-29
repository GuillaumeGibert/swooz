

use strict;
use warnings;
use Cwd;

my $CurrentDirectory = cwd();

our $PBase         = $CurrentDirectory . "/../../";
our $PThird_party  = $PBase . "swooz-3rdparty/";

$ENV{PATH} ="";
$ENV{PATH} = $PThird_party . "ace/ace-6.1.0/bin;" . $ENV{PATH};

$ENV{ICUB_ROOT} = ".";
$ENV{YARP_DATA_DIRS} ="";
$ENV{YARP_DIR} ="";

chdir "bin";
system("simFaceExpressions.exe");
chdir "..";



use strict;
use warnings;
use Cwd;

my $CurrentDirectory = cwd();

our $PBase         = $CurrentDirectory . "/../../";
our $PThird_party  = $PBase . "swooz-3rdparty/";

$ENV{PATH} ="";
$ENV{PATH} = $PThird_party . "ace/ace-6.1.0/bin;" . $ENV{PATH};

$ENV{ICUB_ROOT} = ".";

chdir "bin";
system("emotionInterface.exe");
chdir "..";

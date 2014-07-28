

use strict;
use warnings;
use Cwd;

my $CurrentDirectory = cwd();

our $PBase         = $CurrentDirectory . "/../../";
our $PThird_party  = $PBase . "swooz-3rdparty/";

$ENV{PATH} ="";

$ENV{PATH} = $PThird_party . "ace/ace-6.1.0/bin;" . $ENV{PATH};
$ENV{PATH} = $PThird_party . "gtk/bin;" . $ENV{PATH};
$ENV{PATH} = $PThird_party . "opencv/opencv-2.49/build/bin/Release;" . $ENV{PATH};
$ENV{PATH} = $PThird_party . "openni/openni-1.5.4.0/Lib;" . $ENV{PATH};
$ENV{PATH} = $CurrentDirectory . "/../icub/bin;" . $ENV{PATH};

print $ENV{PATH};

chdir "bin";
system("gyarpmanager.exe");
chdir "..";

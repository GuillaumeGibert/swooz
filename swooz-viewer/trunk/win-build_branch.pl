
use strict;
use warnings;

####################################################################################### CREATE PROJECTS DIRECTORIES
my @directoriesToCreate = ("bin", "lib", "moc", "genUI", "lib/x86", "lib/x86/Release", "lib/x86/Debug", "lib/amd64", "lib/amd64/Release", "lib/amd64/Debug");

foreach (@directoriesToCreate)
{
    my $dirName = $_;

    if(not(-d $dirName))
    {
        mkdir $dirName;
    }
}

my $CFG  = "Release";
my $ARCH = "x86";
my $COMP = "cl";

if(@ARGV > 1)
{
    $CFG   = $ARGV[0];
    $ARCH  = $ARGV[1];
}

print $CFG . " " . $ARCH . " " . $COMP . "\n";

my $setEnvScript = $Env::SWScripts . "others/setEnvMVS.cmd " . $CFG . " " . $ARCH . " " . $COMP;
system("$setEnvScript");

1;



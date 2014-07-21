
use strict;

{
    local @ARGV = ("noPath");
    require "win-init_env_command.pl";
}

my @directoriesToCopy = ("include", "src");
my $xcopyCmd = "\"" . $ENV{SystemRoot} . "/system32/xcopy\" /q /e /y /c ";

foreach (&Env::buildOrder())
{
    my $projectName = $_;
    my $projectFullName = $Env::PBase . $projectName;
    print "[" . $projectFullName . "]\n";

    foreach (@directoriesToCopy)
    {
        my $source_dir = $projectFullName . "/" . $_;
        my $target_dir = $Env::SWDist . $_;
        system($xcopyCmd . "\"" . $source_dir . "\" \"" . $target_dir . "\"");
    }
}


chdir "../dist";

if(not(-d "doc"))
{
    mkdir "doc";
}

chdir "../scripts";

system("doxygen ./Doxyfile");
1;


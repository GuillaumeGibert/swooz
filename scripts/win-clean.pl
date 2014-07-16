
use strict;
use File::Path 'rmtree';

{
    local @ARGV = ("noPath");
    require "win-init_env_command.pl";
}

foreach (&Env::buildOrder())
{
    my $projectName = $_;
    my $projectFullName = $Env::PBase . $projectName;

    print "[clean " . $projectFullName . "]\n";
    if(chdir $projectFullName)
    {
        rmtree("bin");
        rmtree("lib");
        rmtree("genUI");
        rmtree("moc");
        chdir "../scripts";
    }
    else
    {
       die "Error: " . $projectFullName . " doesn't exist. \n";
    }
}

if(chdir $Env::PDist)
{
    print "[clean " . $Env::PDist . "]\n";
    rmtree("bin");
    rmtree("lib");
    rmtree("genUI");
    rmtree("moc");
    chdir "../scripts";
}
else
{
    die "Error: " . $Env::PDist . " doesn't exist. \n";
}

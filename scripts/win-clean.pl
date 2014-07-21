
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

if(chdir $Env::SWDist)
{
    print "[clean " . $Env::SWDist . "]\n";
    rmtree("bin");
    rmtree("lib");
    rmtree("genUI");
    rmtree("moc");
    rmtree("include");

    for (my $ii = 0; $ii < &Env::executablesNumber(); $ii++)
    {
        my $cmdName = &Env::commandFileName($ii) . ".pl";
        unlink $cmdName;
    }

    chdir "../scripts";
}
else
{
    die "Error: " . $Env::SWDist . " doesn't exist. \n";
}



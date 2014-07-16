
use strict;

{
    local @ARGV = ("noPath");
    require "win-init_env_command.pl";
}

foreach (&Env::buildOrder())
{
    my $projectName = $_;
    my $projectFullName = $Env::PBase . $projectName;
    print "[generate doc : " . $projectFullName . "]\n";
    chdir $projectFullName;

    {
        require "win-generate_doc.pl";
    }
    chdir "../scripts";
}

my $xcopyCmd = "\"" . $ENV{SystemRoot} . "/system32/xcopy\" /q /e /y /c ";
foreach (&Env::buildOrder())
{
    my $projectName = $_;
    my $projectFullName = $Env::PBase . $projectName;

    my $source_dir = $projectFullName . "/doc";
    my $target_dir = $Env::PDist . "/doc/*";
    system($xcopyCmd . "\"" . $source_dir . "\" \"" . $target_dir . "\"");
}

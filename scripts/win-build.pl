

use strict;
use warnings;

if ( @ARGV > 0 )
{
  print "Number of arguments: " . scalar @ARGV . "\n";
}

my $ARGNb = 0;
my $CFG  = "Release";
my $ARCH = "x86";

if(@ARGV > 0)
{
    # TODO : check args
    $ARGNb = @ARGV;
    $CFG = $ARGV[0];
}
if($ARGNb > 1)
{
    $ARCH = $ARGV[1];
}

print "Build $CFG, $ARCH\n";


{
    local @ARGV = ("noPath");
    require "win-init_env_command.pl";
}

if($ENV{CUDA_FOUND} eq "no")
{
    print "\n!!!!!!!!!!!!!\nWARNING : CUDA PATH not found, projects using CUDA will not be compiled !\n!!!!!!!!!!!!!\n";
}

#######################################################################################
print "\n";
print '#' x 50 . "\n";
print "## Creating 'dist' folder structure...\n";

if (not(-d  $Env::SWDist)) {
    mkdir $Env::SWDist;
}

my @directoriesToCreate = ("bin", "include", "lib", "data", "doc", "log","examples");

foreach (@directoriesToCreate)
{
    my $dirName = $Env::SWDist . $_;

    if(not(-d $dirName))
    {
        mkdir $dirName;
    }
}

#######################################################################################
print "\n";
print '#' x 50 . "\n";
print "## Call all the projects makefiles...\n";

my @directoriesToCopy = ("bin", "include", "lib", "data");
my $xcopyCmd = "\"" . $ENV{SystemRoot} . "/system32/xcopy\" /q /e /y /c ";


foreach (&Env::buildOrder())
{
    my $projectName = $_;
    my $projectFullName = $Env::PBase . $projectName;
    print "[" . $projectFullName . "]\n";
    chdir $projectFullName;

    {
        local @ARGV = ($CFG, $ARCH);
        delete $INC{"win-build_branch.pl"};
        require "win-build_branch.pl";
    }

    chdir $Env::SWScripts;

    foreach (@directoriesToCopy)
    {
        my $source_dir = $projectFullName . "/" . $_;
        my $target_dir = $Env::SWDist . $_;
        system($xcopyCmd . "\"" . $source_dir . "\" \"" . $target_dir . "\"");
    }
}

#######################################################################################
print "\n";
print '#' x 50 . "\n";
print "## create exe scripts for all projects...\n";
$xcopyCmd = "\"" . $ENV{SystemRoot} . "/system32/xcopy\\\" /q /e /y /c ";

for (my $ii = 0; $ii < &Env::executablesNumber(); $ii++)
{
    my $scriptFileName  = $Env::SWDist . &Env::commandFileName($ii) . ".pl";
    my $fh;
    open($fh, '>', $scriptFileName) or die "Cannot write in " . $scriptFileName;

    if($ENV{CUDA_FOUND} eq "no")
    {
        print $fh "print \"WARNING : CUDA not detected, some projects can not be launched.\n\";";
    }

    # manage args
    print $fh "my \$ARG1 = \"\";\n";
    print $fh "my \$ARG2 = \"\";\n";
    print $fh "my \$ARG3 = \"\";\n";
    print $fh "if(\@ARGV > 0)\n";
    print $fh "{\n  \$ARG1 = \@ARGV[0];\n}\n";
    print $fh "if(\@ARGV > 1)\n";
    print $fh "{\n  \$ARG2 = \@ARGV[1];\n}\n";
    print $fh "if(\@ARGV > 2)\n";
    print $fh "{\n  \$ARG3 = \@ARGV[2];\n}\n";

    print $fh "system(\"\\" . $xcopyCmd . " \\\"" . $Env::PBase . "swooz-config\\\" \\\"" . $Env::SWDist . "data\\\"\");\n";
    print $fh "chdir \"" . $Env::SWScripts . "\";\n";
    print $fh "{\n   local \@ARGV = (\"" . &Env::archExe($ii) . "\");\n";
    print $fh "   require \"win-init_env_command.pl\";\n}\n";
    print $fh "chdir \"../dist/bin\";\n";
    print $fh "system(\"" . &Env::exeFileName($ii) . "\" . \" \" . \$ARG1 . \" \" . \$ARG2 . \" \" . \$ARG3);\n";
    print $fh "chdir \"..\";\n";
    close($fh);
}



use strict;
use File::Path 'rmtree';

if(chdir "../dist")
{
    print "[doc clean ../dist]\n";
    rmtree("doc");
    rmtree("include");
    rmtree("src");
    chdir "../scripts";
}
else
{
    die "Error: ../dist doesn't exist. \n";
}

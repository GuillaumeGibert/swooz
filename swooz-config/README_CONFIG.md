**DO NOT add files of this directory in the svn.**

All the files of this directory will be copied in /dist/data.

The purpose is to allow extern files modifications without a recompilation of the project.
When the **win-build.cmd** script is called, the data directory of each swooz project is copied to ../dist/data.
So when a file is modified, a new call to **win-build.cmd** is needed to update /dist/data.

But when a cmd project file is launched (like SWToolkit.cmd), all this directory is copied to ../dist/data,
so if an other file with the same name exists he will replace the original one.




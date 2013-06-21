///////////////////////////////////
//    Jacob Meacham              //
//	Ego Engine		 //
//   jmeacha3@uoregon.edu        //
// http://uoregon.edu/~jmeacha3  //
///////////////////////////////////

Requires the newest version of DirectX 9 to run.  If you see an error about missing d3dx9_35.dll, you need
to update your DirectX.

This is the code to compile the Ego engine.  To create a game using this engine, follow the
engine structure detailed at http://uoregon.edu/~jmeacha3/egoEngine.html and look at the sample game
at http://uoregon.edu/~jmeacha3/egoGame.zip.
 
Both of these should compile correctly with the newest version of Visual C++, provided that you set up 
the DirectX headers correctly.  To do so in Visual C++, go to 
Tools->Options->Projects and Solutions->VC++ Directories->Include and add a line for DirectX 9
header files.  Do the same for Library.  Finally, in Project->Properties->Linker->Input, under
additional dependencies, add this line: dxguid.lib dinput8.lib d3d9.lib d3dx9.lib.

Also included is the code and .exe for the utility BitmaptoCollision, which can turn a bitmap into a
collision map for pathfinding.

Enjoy!
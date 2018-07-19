# Sudoku

A general sudoku solving app using backtracking and pruning of candidate values. Details of how the algorithm works can be found inside the source code.

The app uses a simple cross-platform open-gl based library (of my own creation) for its user interface. The files supporting this library are all prefixed with `sw_` and most are contained in the `sw/` folder. 

Project files to build the app for iOS are included. It is possible to build the project on Android Studio. However, differences in the layout of folders makes it tricky to add directly to git.

You can build the project in Android Studio if you set it up manually. Here are the steps to do this.

Create a new Android Studio project. Set the fields as follows ..

- Set Application name to Sudoku.
- Set Company domain to qlik.com
- Make sure the Include C++ support box is checked.
- Second page you can leave as is.
- Keep the selection on Empty Activity
- Change the Activity Name to AppActivity
- Leave layout name as is
- Leave the C++ Standard library as Toolchain Default    

Once the project has been created, you will have to do the following edits and file updates

Create an assets folder by right clicking on the app folder and choosing *New->Folder->Assets Folder*. Leave the Target Source Set as main

Delete the native-lib.cpp file in the `cpp/` folder.

Copy the files

```
   {main-project}/android/src/com/qlik/sudoku/AppActivity.java
   {main-project}/android/src/com/qlik/sudoku/AppView.java
   {main-project}/android/src/com/qlik/sudoku/AppJni.java
   {main-project}/android/src/com/qlik/sudoku/AppTexture.java
```
to
 
```
   {android-project}/app/src/main/java/com/qlik/sudoku/
``` 
Overwrite the default AppActivity.java file that was created by Android Studio.

Copy the folders

```
   {main-project}/android/jni/
   {main-project}/src/
   {main-project}/sw/
```   
   to 
   
```
{android-project}/app/src/main/cpp/
```
 
You should now have the following layout in your `cpp/` folder
```   
   {android-project}/app/src/main/cpp/jni/
   {android-project}/app/src/main/cpp/src/
   {android-project}/app/src/main/cpp/sw/
```

Copy the files 

```
	{main-project}/sw_app_msg.h
	{main-project}/sw_app.c
	{main-project}/sw_cfg.h
```	
to 

```
	{android-project}/app/src/main/cpp/
```

Replace `{android-project}/app/CMakelists.txt` with the version from `{main-project}/android/CMakelists.txt`

Copy `{main-project}/tex/tex_main.png` to the `{android-project}/app/src/main/assets/` folder

After copying all of the files, choose

	*File/Sync Project with Gradle Files*
	*Build/Refresh Linked C++ Projects*
	
If you run on the emulator you may have to make a few tweaks to get it to run. More specifically adding more memory allocated to the emulated device.




                            README  
                  (C)Örebro University 2004-09-30
                  ===============================


      This folder contains workspace and project files for 
      Microsoft Visual C++ 6.0.
      
      The "win_develop" folder is located directly under 
      the "varkon_1.18A" folder:
      
      varkon_1.18A
        |-app
        |-bin
        |-cnf
        |-com
        |-erm
        |-lib
        |-man
        |-mdf
        |-pid
        |-sources
        |-tmp
        |-win_develop


       Start Microsoft Visual C++ and open workspace "varkon95.dsw".
       
       There are four different build configurations, which are activated by 
       entering the menu: Build -> set active configuration.
       
       1) Mbsc95 - Win32 Release
          builds Mbsc.exe, release version (located in varkon_1.18A/bin)
       
       2) Mbsc95 - Win32 Debug
          builds Mbsc.exe, debug version 
       
       3) Varkon95 - Win32 Release
          builds Varkon95.exe, release version (located in varkon_1.18A/bin)
      
       4) Varkon95 - Win32 Debug
          builds Varkon95.exe, debug version

       N.B 
       Before you run the debug version of Varkon95.exe for the first time
       on your computer, you should always start the release version 
       located in varkon_1.18A/bin once. This is to get the environment 
       variables set up correctly. See the varkon Users manual -> varkon95.exe.

       This is the recomended steps to perform to compile varkon from sources and
       get Varkon working for the first time:

       1) Set active configuration to Mbsc95 - Win32 Release and build Mbsc95

       2) Set active configuration to Varkon95 - Win32 Release and build Varkon95

       3) Run Varkon95.exe (located in %VARKON_ROOT%\bin). Environment variables
          will now be addad to your system. Exit Varkon.

       4) Build the varkon lib directory from MBS sources by running make_lib.bat 
          (located in this directory).

       Now varkon is ready for running.

       If you now would like to run the debug version from inside Visual C++,
       you must first restart Visual C++ (otherwise, Visual C++ will not get the new
       environemt variables into its process environment).
       Then change to configuration: Varkon95 - Win32 Debug.




                                    Good Luck

                                    The CAD research group
                                    Department of Technology
                                    Örebro University
                                    S-70182 Örebro
                                    SWEDEN

                                    URL:    http://www.oru.se/tech/cad
                                  

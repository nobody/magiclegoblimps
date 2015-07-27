# Introduction #

UTD

Senior Design Class Spring 2010
Group 2A

Remote Monitoring System

User's Manual

I Table of Contents
II Project overview
III Client Manual
IV Server Manual
V Robot Manual


# Details #

> INSTALLATION

  1. 
> > Installation Requirements
      1. 
> > > > Four physical machines.
          1. 
> > > > > > Multiple physical computers are strongly recommended, as some processes are very processor intensive.

> > > > > 2.
> > > > > > Web Server
              1. 
> > > > > > > > Linux OS required.

> > > > > > > 2.
> > > > > > > > mySQL version 5.1.37 or later packaged with phpMyAdmin

> > > > > > > 3.
> > > > > > > > Apache 2.2.12 or later

> > > > > > > 4.
> > > > > > > > Php 5.2.10-2 or later

> > > > > 3.
> > > > > > Video Server
              1. 
> > > > > > > > Linux OS required

> > > > > > > 2. Python 2.6.x
> > > > > > > 3. MySQL-python 1.2.3c1
> > > > > > > 4. FFmpeg/FFserver (bundled - always use latest version)
> > > > > > > 5. Apache HTTP Server - version 2.x

> > > > > 4.
> > > > > > Robot Server
              1. 
> > > > > > > > Windows system with bluetooth capability required.

> > > > > > > 2. Must be in transmission range of robots.
> > > > > > > 3. OpenCV 2.0 or later
> > > > > > > 4. Anders Mindstorms C++ Library 2.1 or later for Windows
> > > > > > > 5. Visual Studio 2008

> > > > > 5.
> > > > > > Controller Server
              1. 
> > > > > > > > Linux OS required.

> > > > > > > 2.
> > > > > > > > gcc 4.4 or later

> > > > > > > 3.
> > > > > > > > make 3.81 or later

> > > > > > > 4. boost libraries version 1.42.0 or greater
> > > > > > > 5. mysqlcppconn library version 1.0.5

> > > 2.
> > > > A Wireless router on the same network as the Robot controller.
          1. 
> > > > > > Recommended 802.11g

> > > 3.
> > > > Desired number of Robots
          1. 
> > > > > > This document assumes robots are available as completed units with all functionality of robots built by our group.

> > > 4.
> > > > Desired number of uniquely colored objects

> 2.
> > Installing the Web Server
      1. 
> > > > Install all prerequisites

> > > 2.
> > > > From the clientserver.tar.gz extract everything into web directory.

> > > 3.
> > > > Execute database.sql in phpmyadmin.

> 3.
> > Installing the Robot Server
      1. Install the prerequisites
> > > 2.  Load the project into Visual Studio 2008

> 4.
> > Installing the Controller Server
      1. Install all prerequisites
> > > 2.
> > > > From the controllerserver.tar.gz extract the folder "Controller" into the desired directory on the Controller Server machine.

> > > 3.
> > > > From that directory run "./install"

> 5.
> > Installing the Video Server
      1. 
> > > > Download the Video Delivery Server code from:
> > > > http://code.google.com/p/magiclegoblimps/files/VideoDelivery.tar.gz

> > > 2. Extract the tarball:
> > > > $> tar -xzf VideoDelivery.tar.gz

> > > 3. Change to the VideoDelivery directory:
> > > > $> cd VideoDelivery/

> > > 4. Modify the settings.py file as needed. See below for a full description of all the settings, see the Video delivery server settings section at the end of this document. At a minimum, you need to configure the DEBUG, QOS\_SERVER\_URL, QOS\_SERVER\_PORT, and CURRENT\_IP settings. Some others that you may need to configure include the ARCHIVE\_DIR, ARCHIVE\_FEED\_URLS, MySQL\_HOST, MySQL\_USER, MySQL\_PASS, and MySQL\_DATABASE settings.

STARTING UP THE SYSTEM


  1. 

> > Web Server
      1. 
> > > > Turn server on. All required services will autostart.

> 2.
> > Controller Server
      1. 
> > > > from the "Controller" directory, run the program "./ServerController"

> > > 2.
> > > > Server Controller will start up.

> 3.
> > All other components can be started in arbitrary order, or concurrently.
      1. 
> > > > Video Server
          1. Navigate to the VideoDelivery directory
> > > > > 2.
> > > > > > Wait for the client group's database server and the Controller server to be running.

> > > > > 3. Run the video delivery server:
> > > > > > $> python vidcontrol.py &


  1. 

> > Robot Server
      1. Run the project in Visual Studio
> > > 2. Commands to connect to servers and robots are below

USING THE SYSTEM

> Client Side
Open your favorite browser, and if you are from within the UTD network, type http://www.tinyurl.com/zoocam.


  1. 
> > Guest Users
      1. 
> > > > No system registration necessary.

> > > 2.
> > > > Voting/Requests
          1. 
> > > > > > Use navigation link, and browse to animal sub page.
              1. 
> > > > > > > > Use scolling animal sub navigation, and choose animal.

> > > > > > > 2.
> > > > > > > > Click on vote button to make request.

> > > > > > > 3.
> > > > > > > > Voting continues while user session is active on that animal sub page.  If user navigates aways from the animal page, the request ends.

> 2.
> > Registered Users
      1. 
> > > > Registration
          1. 
> > > > > > Click sign up link from home page.

> > > > > 2. Voting/Requests
            1. 
> > > > > > > Use navigation link, and browse to animal sub page.
                1. 
> > > > > > > > > Use scolling animal sub navigation, and choose animal.

> > > > > > > > 2.
> > > > > > > > > Click on vote button to make request.

> > > > > > > > 3.
> > > > > > > > > Voting continues while user session is active on that animal sub page.  If user navigates aways from the animal page, the request ends.

> > > > > 3.
> > > > > > Camera Control
              1. 
> > > > > > > > Available from animal sub page when animal request is being serviced
                  1. 
> > > > > > > > > > Zoom In/Out
                      1. 
> > > > > > > > > > > > Software only

> > > > > > > > > 2.
> > > > > > > > > > Tilt Up/Down
                      1. 
> > > > > > > > > > > > Software only

> 3.
> > Administrative Users
      1. 
> > > > User Functions
          1. 
> > > > > > Adding (Users->Add User)

> > > > > 2.
> > > > > > Editing (Users->Select User->Edit)

> > > > > 3.
> > > > > > Deleting (Users->Select User->Delete)

> > > 2.
> > > > Animal Functions
          1. 
> > > > > > Adding - From the Animal in the Robot Controller, add metadata (Description, images from the administration page on the client.

> > > > > 2.
> > > > > > Editing - Edit metadata on the administration page on the client.

> > > > > 3.
> > > > > > Delete - Remove animal from the table on the Controller Server.

> > > 3.
> > > > Camera/Robot Functions
          1. 
> > > > > > Controlling a Robot
              1. 
> > > > > > > > Select Robot from map, default is Robot #1

> > > > > > > 2.
> > > > > > > > Control is implicit

> > > > > > > 3.
> > > > > > > > Turn Left
                  1. 
> > > > > > > > > > Only works at intersection

> > > > > > > > > 2.
> > > > > > > > > > 90 degree turn

> > > > > > > 4.
> > > > > > > > Turn Right
                  1. 
> > > > > > > > > > Only works at intersection

> > > > > > > > > 2.
> > > > > > > > > > 90 degree turn

> > > > > > > 5.
> > > > > > > > Forward
                  1. 
> > > > > > > > > > Moves to next intersection

> > > > > > > 6.
> > > > > > > > Release
                  1. 
> > > > > > > > > > Explicit, currently remains under Admin control indefinetly.

> > > > > 2.
> > > > > > Controlling a Camera associated with a Robot.
              1. 
> > > > > > > > Zoom In/Out
                  1. 
> > > > > > > > > > Software Only

> > > > > > > 2.
> > > > > > > > Tilt Up/Down
                  1. 
> > > > > > > > > > Software Only

> > > > > > > 3.
> > > > > > > > Pan Left/Right
                  1. 
> > > > > > > > > > Robot pans camera

> > > 4.
> > > > Content Management System, CMS, to manage art assets associated with objects.  At future development iterations, a CMS should be added.


Video delivery server settings

The following describes all of the available settings in the settings.py file for the video delivery setting. If the description says not to modify the setting, you probably should not modify it unless you really, really, really know what you are doing. The settings.py file is a normal Python source code file, so any Python code is acceptable.

DEBUG
Whether or not the project in DEBUG mode. This should be set to False for a production environment, or True for testing. When in debug mode, the program uses dummy programs that do not do anything in place of launching ffmpeg/ffserver processes for transcoding/streaming video.

ROOT\_DIR
A list of directories in which the program will run. It chooses the first one that exists on your system, and defaults to the current directory if all fail. This allows us to accomodate different installation paths in different testing environments.

LOGFILE
Relative path to the server's runtime log. You should create all directories listed in this path before the program runs.

TRANSCRIPT\_FILE
Relative path to the log that records all of the data sent between the video delivery server and the quality of service (QoS) server.

QOS\_SERVER\_URL
The host name or IP address for the QoS server.

QOS\_SERVER\_PORT
The port on which to connect to the QoS server.

MAX\_CONNECTION\_ATTEMPTS
Set the number of times to retry a connection to the QoS server if the initial connection fails.

CONNECTION\_RETRY\_INTERVAL
The time in seconds between connection attempts.

CURRENT\_IP
The current host name or IP address for the video delivery server. This can be obtained by running the ifconfig command.

FEEDER\_URLS
Format of the URLs used internally by FFserver during transcoding. Don't modify this unless you know what you're doing.

LIVE\_FEED\_URLS
Format of the URLs for the live Flash video streams. Don't modify this unless you know what you're doing.

ARCHIVE\_FEED\_URLS
The base URL including directory path in which the archive videos and thumbnails are stored. This URL should correspond to the physical directory specified in the ARCHIVE\_DIR setting.

ARCHIVE\_CSV\_FILE
Deprecated. We are instead updating the client group's database directly.

ARCHIVE\_HTML
Testing only. An HTML file that lists links to all of the archive videos.

ARCHIVE\_DURATION
The amount of time (in seconds) to record for archive videos.

ARCHIVE\_QOS\_THRESHOLD
If the QoS metric for an object in a camera feed exceeds this value [0.0-1.0], an archive video will be recorded.

ARCHIVE\_THUMB\_SIZE
The thumbnail size in pixels for archive video thumnails. The format is a python string such as 'WxH', where W is the width in pixels and H is the height in pixels.

CONFIG\_TEMPLATE
The template file used to create FFserver configuration files. Don't mess with this unless you're an FFserver guru. You can even modify the contents of the configuration template file if you really, really know FFserver.

CONFIG\_FILE\_DIR
The directory in which the CONFIG\_TEMPLATE file is located.

ARCHIVE\_DIR
The directory in which the archive videos and thumbnails are stored. This should be in a web accessible directory (i.e. beneath Apache's server root directory), and the owner under which the video delivery process is executing needs to have permissions to write to this directory. The easiest way to do this is change the owner of the archive directory to be the same as the user running the video delivery server.

CONFIG\_NAME
Format for FFserver configuration files. Leave this alone.

STREAM\_NAME
Used to name/identify live Flash streams. Leave this alone.

FEED\_NAME
Used internally by FFserver to track streams being transcoded.

FFMPEG\_ARGS
Specify arguments for transcoding video streams. Leave this alone  unless you are familiar with FFmpeg. This is a Python list containing the same arguments you would give on the command line. The input-stream and output-stream arguments will be replaced by the actual input/output streams determined by the video delivery server.

FFSERVER\_ARGS
Specify arguments for running FFserver instances. Don't mess with this. FFserver doesn't have any other arguments anyways.

MySQL\_HOST
The host name of the client group's MySQL database.

MySQL\_USER
The user name for the client group's MySQL database.

MySQL\_PASS
The password for the client group's MySQL database.

MySQL\_DATABASE
The database for name the client group's MySQL database


Robot Controller
Run the project in Visual Studio 2008. The list of acceptable commands is as follows:
connect:
Use: connect 

&lt;ip&gt;


Connects to the backend server at 

&lt;ip&gt;

.
addrobot:
Use: addrobot 

&lt;port&gt;

 

&lt;ip&gt;

 

&lt;cameratype&gt;


Connects to a robot on com port 

&lt;port&gt;

 with camera ip 

&lt;ip&gt;

. 

&lt;cameratype&gt;

 is a boolean flag (true/false) specifying type of camera (D-Link or Cisco).
removerobot:
Use: removerobot 

&lt;port&gt;


Removes robot using com port 

&lt;port&gt;


command:
Use: command 

&lt;port&gt;

 

&lt;command&gt;


Issues 

&lt;command&gt;

 to the robot on 

&lt;port&gt;

. A list of valid commands is as follows:

  * forward - advances the robot to the next intersection directly in front of it
  * right - turns the robot to the right
  * left - turns the robot to the left
  * turnaround - turns the robot around 180 degrees to move in the opposite direction
  * pan 

&lt;degrees&gt;

 - rotates the camera the number of degrees specified in 

&lt;degrees&gt;



display:
Use: display 

&lt;port&gt;


Displays the camera feed of the robot on com port 

&lt;port&gt;


stopdisplay:
Use: stopdisplay 

&lt;port&gt;


Stops displaying the feed from the robot on com port 

&lt;port&gt;


startprogram:
Use: startprogram
stopprograms:
Use: stopprograms
removeobject:
Use: removeobject 

&lt;id&gt;


Removes target 

&lt;id&gt;

 from the list of trackable targets
quit:
Use: quit
Exits the application



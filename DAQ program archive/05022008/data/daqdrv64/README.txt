
README.txt

This is the readme file for the daqkit distribution kit.

Daqkit contains a high performance device driver (daqdrv) for data acquisiton,
a software library (daqlib) that presents a simple high layer interface to
the driver and device functions, and sample source codes.

Daqkit is provided as a single tar-gzip file (.tgz) that includes the driver
source codes, the software library, source code examples and documentation.

     daqdrv/	 the device driver source files
     daqlib/	 the software library
     samples/	 example source codes
     daqlib.pdf	 documentation for the high level library
     daqdrv.pdf	 documentation for the device driver
     README.txt	 this file

Daqkit is written by Dr. Mitch Nelson, Sensor Realtime, LLC

Communicating with the author:
------------------------------

Feature requests are welcome, as are requests for help. Please forward
these requests to us through your vendor (General Standards Corporation).

We hope that there are no bugs, but if you feel that you found one, please
collect whatever information you can in text files or screen images, and
send them to us through your vendor. Please be sure to include a copy of the
kernel log (for example, # dmesg > klog.txt). Also, see building with debug,
below.

Installation:
-------------

1) Install the data acquision (daq) card.

   See the documentation for your card for installation instructions.

   After the card is installed, boot the machine.

2) Create a directory where you would like to locate the daqkit files.

   For example,

   # mkdir /daqkit

3) Unpack the distribution to the directory.

   # mv daqkit070115.tgz /daqkit

   # cd /daqkit
   
   # tar -xzf daqkit0070115.tgz

4) Build the device driver and the sample programs

   # ./buildkit.sh

5) Start the device driver.

   # daqdriver/daqdrvstart.sh

   (you must have root privilages to do this)

6) Run a sample program to verify the driver installation.

   # samples/exercise       (this runs all of the analog inputs and outputs)


Building with Debug
-------------------

To enable debug messages in the driver,

1) cd into the driver directory

   # cd /daqkit/daqdriver

2) edit the Makefile

   # myfavoriteeditor Makefile

   Find the lines at the begging of the file

	# comment the following line to disable debugging
	#DEBUG = y

  and remove the "#" from in front of the word DEBUG.

3) build the driver

   # make

4) reload the driver

   # ./daqdrvstop.sh

   # ./daqdrvstart.sh


Caution: This turns on a lot of debug messages. They all go into the
	 kernel message log. After collecting the debug messages that
	 you need, it is best to re-comment the line in the Makefile
	 and rebuild and reinstall the driver as soon as convenient.

   

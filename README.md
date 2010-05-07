# Barbelo S60 geolocation wifi sectool

## Description
Barbelo was a wireless (WiFi) security related toolset for Symbian S60 v3 written by [Andrea Bittau](mailto:a.bittau@cs.ucl.ac.uk)

It supports:
- Standart netstumbler/kismet like functionality. 
- GPS support to map networks (in combination with GPSd). 
- Logging in Kismet (XML) format. 

## This fork
The program, oringinaly store the logs to `e:\barbelo\logs` path without the posibility of change it, so if no memory card exist on the phone, the program crash. 

**In this fork I changed** it to `c:\DATA\warwalk` to store the logs into the phone internal memory at the warwalk folder. 

## Usage
- If you want GPS support, start GPSd prior to Barbelo.
- You can select a network in the scan/map tab by pressing up or down.
- You can view the details of a network in the scan/map tab by "clicking" on it _(pressing "fire" key between arrows on N95)_.
- You can zoom in/out in map view by pressing hash/star respectively.
- Logs go into e:\barbelo\logs in the original version
- Logs go into c:\DATA\warwalk in this forked version

## Screenshots
![Screenshot](https://raw.githubusercontent.com/multipetros/barbelo/master/.github/screenshot.jpg)

## Download
Barbelo **C Drive release**:
- [barbelo-v0.3-cdrive.sis](https://github.com/multipetros/barbelo/releases/download/v0.3/barbelo-v0.3-cdrive.sis) (07/05/10). 

Barbelo **original releases**:
- [barbelo-v0.3.sisx](https://github.com/multipetros/barbelo/releases/download/v0.3/barbelo-v0.3.sisx) (31/05/08). _Re-released on 01/06/08 to support pre-FP1 phones (e.g., e65) - thanks to J. Aznar for debugging._
- [barbelo-v0.2.sisx](https://github.com/multipetros/barbelo/releases/download/v0.3/barbelo-v0.2.sisx) (27/04/08).
- [barbelo-v0.1.sisx](https://github.com/multipetros/barbelo/releases/download/v0.3/barbelo-v0.1.sisx) (14/03/08).

GPSd is required for **GPS functionality**:

- [GPSd-v0.2.jar](https://github.com/multipetros/barbelo/releases/download/v0.3/GPSd-v0.2.jar) (06/05/08).
- [GPSd-v0.1.jar](https://github.com/multipetros/barbelo/releases/download/v0.3/GPSd-v0.1.jar) (27/04/08).

## License
This program is free software distributed under the GNU GPL 2, for license details see at license.txt file, distributed with this program, or see at http://www.gnu.org/licenses/.
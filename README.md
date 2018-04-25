# SipNPuff_Mouse

This is a mouse for Quadriplegics and other mobility impaired people, intended to be used with one's mouth. 
This is the repository for code, STL files, and any guides for the mouse, which I show how to build at https://www.instructables.com/id/The-Sup-a-Mouse-for-Quadriplegics-Low-Cost-and-Ope/.

This is a mouse input device intended to be used by Quadriplegics and other mobility impaired people. 
It's a combination of a joystick and a sip-n-puff controller, and is controller by one's mouth. 

Hardware:
Arduino Pro Micro, normal joystick module, and an MPXV7002DP (-0.3 to 0.3 PSI) pressure sensor, enclosed in a 3D printed case.

Software:
A single sketch running on the Arduino, samples values from the joystick and pressure sensor and inputs mouse commands based off that.

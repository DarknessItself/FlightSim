CSCI 3161 Project - Flight Simulator
By Matthew Thomas, mthomas@cs.dal.ca
Banner: B00558461

Ensure that the resources and textures folders are in the same location as the application before running.
Precompiled code for Windows with the resources/textures is in the "Release" folder.
Source code is found in the "src" folder.


Description:
 * Creates an area to fly around with in a propeller based plane.
 * Generates 3 randomly generated islands.
 * Islands used midpoints for generation and some "jitter" to make the peaks not always straight up
 * Generates normals for the mountains per vertex.
 
Features:
 * w - toggles between frame and solid rendering
 * f - toggles between fullscreen and windowed mode
 * b - toggle fog on or off
 * s - toggle between grid and sea+sky
 * m - toggles mountains on and off
 * t - toggles textures for mountains on and off
 * q - quit

 * Standard controls:
 * 	Page Up - Increase Speed
 * 	Page Down - Decrease Speed
 * 	Up - Raise elevation of plane
 * 	Down - Lower elevation of plane
 * 	mouse on left side of screen - Turn plane to the left
 * 	mouse on right side of screen - Turn plane to the right

 * Bonuses:
 * 	f1 - toggle between standard and alternate controls
 * 	f2 - toggle between standard fog and cloud-like fog
 * 	z - shoot bullets
 * 	Spedometer (Shows speed roughly, green bar bottom left)
 * 	Altometer (Shows altitude roughly, blue bar bottom right)
 * 	Crash into the sea and there is an explosion + plane dies
 * 	r - respawn and generate new random world
 *	crosshair

 * Alternate controls:
 *  mouse on top of screen - Tilt plane and camera up
 *  mouse on bottom of screen - Title plane and camera down
 *  scroll wheel - increase or decrease speed

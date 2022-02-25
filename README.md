# FirstPersonEngine

See a quick preview of this project on my YouTube channel: https://www.youtube.com/watch?v=pGJHIyOd-CM

### Comments
I made this project before I used Github, without the goal in mind of sharing the source code with anybody. Thus, documentation is lacking.

### Description
This is a 3d first person video game, made entirely from scratch. The player will attempt to navigate a randomly generated 
maze to find the goal, which is green.

I used this project to learn the basics of the windows API in c++ for creating windows applications. I also learned an interesting and
surprisingly simple way of rendering a 3d game. For every column of pixels in the player's view, the game will measure the distance from
the player to the first wall in that direction. As the distance to the wall decreases, the height of the wall in that column of pixels increases.
I'm still not convinced that I got the method totally right, because sometimes straight
walls can appear curved.

### How to Use
This project was made in Visual Studio. I recomend cloning and compiling the repository from there. Use the arrow keys to move around. I 
did not handle input from the player very well, so you can only press one key at a time and the player will not start moving immediately.

### Resources
I got my method for rendering the game from this youtube tutorial.
https://www.youtube.com/watch?v=xW8skO7MFYw

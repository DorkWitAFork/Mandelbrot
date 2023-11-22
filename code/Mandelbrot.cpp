#include <iostream>
#include "ComplexPlane.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;


int main()
{
    // Get the desktop resolution
    VideoMode desktop = VideoMode::getDesktopMode();
    int width = desktop.width / 2;
    int height = desktop.height / 2;
 
    // Make the window
    RenderWindow window(VideoMode(width, height), "Mandelbrot");

    // Make the plane
    ComplexPlane plane(width, height);

    // Now our font and the text
    Font font;
    Text text;
	font.loadFromFile("heavyhea2.ttf");
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(Color::Green);
    text.setPosition(10, 10);

    // Begin the main loop
    while (window.isOpen())
    {
		// Handle Input segment
		Event event;
        while (window.pollEvent(event))
        {
			// Handle Event::Closed event to close the window
			if (event.type == Event::Closed)
				window.close();

			
            if (event.type == Event::MouseButtonPressed)
            {
				// Right click == zoomOut, then set the center of the plane whereever they clicked
                if (event.mouseButton.button == Mouse::Right)
                {
					plane.zoomOut();
					plane.setCenter(Mouse::getPosition(window));
					plane.updateRender();
				}

				// Left click == zoomIn, then set the center of the plane wherever they clicked
                if (event.mouseButton.button == Mouse::Left)
                {
					plane.zoomIn();
					plane.setCenter(Mouse::getPosition(window));
					plane.updateRender();
				}

				
			}

			// Track the movement of the mouse to display the coordinates
            if (event.type == Event::MouseMoved)
            {
				// Continuously call this as they move the mouse so set the location and then we can display the coordiantes
				plane.setMouseLocation(Mouse::getPosition(window));
			}

			// Press escape = close window
			if (Keyboard::isKeyPressed(Keyboard::Escape))
				window.close();
		}

		// Update the scene
		plane.updateRender();
		plane.loadText(text);

		// Draw the scene
		window.clear();
		window.draw(plane);
		window.draw(text);
		window.display();
	}	

  

    return 0;
}



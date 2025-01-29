#include "ComplexPlane.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <complex>

using namespace std;

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight) {
	m_pixel_size.x = pixelWidth;
	m_pixel_size.y = pixelHeight;
	m_aspectRatio = (float)pixelHeight / (float)pixelWidth;
	m_plane_center = { 0,0 };
	m_plane_size = { BASE_WIDTH, BASE_HEIGHT * m_aspectRatio };
	m_zoomCount = 0;
	m_state = State::CALCULATING;
	m_vArray.setPrimitiveType(Points);
	m_vArray.resize(pixelWidth * pixelHeight);
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const {
	target.draw(m_vArray);
}

void ComplexPlane::updateRender() {
	// Only do this if it's calculating
	if (m_state == State::CALCULATING) {
		// rows
		for (int i = 0; i < m_pixel_size.y; i++) {
			// columns
			for (int j = 0; j < m_pixel_size.x; j++) {
				// store the vertex
				m_vArray[i * m_pixel_size.x + j].position = { (float)j,(float)i };
				Vector2f coord = mapPixelToCoords({ j,i }); // its position on the plane
				int count = countIterations(coord);
				Uint8 r, g, b;
				iterationsToRGB(count, r, g, b); // reference passing
				m_vArray[j + i * m_pixel_size.x].color = { r,g,b }; // now set the calculated colors for that vertex
			}
		}
		// set it to display at the end to stop
		m_state = State::DISPLAYING;
	}
}

void ComplexPlane::zoomIn() {
	m_zoomCount++;
	float x_size = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
	float y_size = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
	m_plane_size.x = x_size; // Keeping things separate for my own readability
	m_plane_size.y = y_size;
	m_state = State::CALCULATING; // Revert to calculating so it can redraw at the new spot
}

void ComplexPlane::zoomOut() {
	m_zoomCount--;
	float x_size = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
	float y_size = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
	m_plane_size = { x_size, y_size };
	m_state = State::CALCULATING; // Revert to calculating so it can redraw at the new spot
}

void ComplexPlane::setCenter(Vector2i mousePixel) {
	/*
	The crucial part : setting the center of the plane to the coordinates of the mouse,
	wherever they click will be the new center 
	*/
	m_plane_center = mapPixelToCoords(mousePixel);
	m_state = State::CALCULATING;
}

void ComplexPlane::setMouseLocation(Vector2i mousePixel) {
	// Not much here, just mapping again
	m_mouseLocation = mapPixelToCoords(mousePixel); 
}

void ComplexPlane::loadText(Text& text) {
	stringstream ss;
	ss << "Mouse Location: (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")" << endl;
	ss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")" << endl;
	ss << "Zoom Level: " << m_zoomCount << endl;
	ss << "Iterations: " << MAX_ITER << endl;
	string display_text = ss.str(); // I like to store it in a separate variable then pass that
	text.setString(display_text);
}


size_t ComplexPlane::countIterations(Vector2f coord) {
	// Create our complex number from the coordinates
	complex<float> c;
	c.real(coord.x); // Kept this separate for dubugging/readability
	c.imag(coord.y);

	complex<float> z(0, 0); // Now to create our z value, which represents the current iteration
	size_t count = 0; // To keep track of how many iterations we've done

	// This goes until we either get past 2, or we read the maximumn iterations
	while (abs(z) < 2 && count < MAX_ITER) {
		z = z * z + c;
		count++;
	}
	return count;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b) {
	// set to black if count eaches max_iter, meaning it didn't escape.
	if (count == MAX_ITER) {
		r = 0;
		g = 0;
		b = 0;
	} else {
		// establish the region and color.
		int region = count / (MAX_ITER / 5);
		int color = count % 5;

		switch (region) {
		case 0:
			r = 255 - color * 255 / (MAX_ITER / 5);
			g = 0;
			b = 255;
			break;
		case 1:
			r = color * 255 / (MAX_ITER / 5);
			g = 0;
			b = 255;
			break;
		case 2:
			r = 255 - color * 255 / (MAX_ITER / 5);
			g = 255;
			b = 0;
			break;
		case 3:
			r = 0;
			g = 255;
			b = color * 255 / (MAX_ITER / 5);
			break;
		case 4:
			r = 255;
			g = 0;
			b = 255 - color * 255 / (MAX_ITER / 5);
			break;
		}
	}
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel) {
	// Help us find their position relative to the size of the window
	//float windowPosX = (mousePixel.x - 0) / static_cast<float>(m_pixel_size.x);
	//float windowPosY = (mousePixel.y - m_pixel_size.y) / static_cast<float>(-m_pixel_size.y);

	// Then we add the offsets to get the range [0,2]
	//float planePosX = relativePosX * m_plane_size.x;
	//float planePosY = relativePosY * m_plane_size.y;

	// Calculate the offsets from the center of the plane
	//float offsetX = (m_plane_center.x - m_plane_size.x) / 2.0;
	//float offsetY = (m_plane_center.y - m_plane_size.y) / 2.0;

	// Now add those offsets to get the exact spot on the plane
	//float x = scaledPosX + offsetX;
	//float y = scaledPosY + offsetY;
	
	float x = (mousePixel.x - 0) / static_cast<float>(m_pixel_size.x) * m_plane_size.x + (m_plane_center.x - m_plane_size.x) / 2.0;
	float y = (mousePixel.y - m_pixel_size.y) / static_cast<float>(-m_pixel_size.y) * m_plane_size.y + (m_plane_center.y - m_plane_size.y) / 2.0;

	
	return { x, y };
}



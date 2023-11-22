#include "ComplexPlane.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <complex>

using namespace std;

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
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

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
	target.draw(m_vArray);
}

void ComplexPlane::updateRender()
{
	if (m_state == State::CALCULATING)
	{
		for (int i = 0; i < m_pixel_size.y; i++)
		{
			for (int j = 0; j < m_pixel_size.x; j++)
			{
				m_vArray[i * m_pixel_size.x + j].position = { (float)j,(float)i };
				Vector2f coord = mapPixelToCoords({ j,i });
				int count = countIterations(coord);
				Uint8 r, g, b;
				iterationsToRGB(count, r, g, b);
				m_vArray[j + i * m_pixel_size.x].color = { r,g,b };
			}
		}
		m_state = State::DISPLAYING;
	}
}

void ComplexPlane::zoomIn()
{
	m_zoomCount++;
	float x_size = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
	float y_size = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
	m_plane_size.x = x_size;
	m_plane_size.y = y_size;
	m_state = State::CALCULATING;
}

void ComplexPlane::zoomOut()
{
	m_zoomCount--;
	float x_size = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
	float y_size = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
	m_plane_size = { x_size, y_size };
	m_state = State::CALCULATING;
}

void ComplexPlane::setCenter(Vector2i mousePixel)
{
	m_plane_center = mapPixelToCoords(mousePixel);
	m_state = State::CALCULATING;
}

void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
	m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(Text& text)
{
	stringstream ss;
	ss << "Mouse Location: (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")" << endl;
	ss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")" << endl;
	ss << "Zoom Level: " << m_zoomCount << endl;
	ss << "Iterations: " << MAX_ITER << endl;
	string display_text = ss.str();
	text.setString(display_text);
}


size_t ComplexPlane::countIterations(Vector2f coord)
{
	complex<float> c;
	c.real(coord.x);
	c.imag(coord.y);
	complex<float> z(0, 0);
	size_t count = 0;
	while (abs(z) < 2 && count < MAX_ITER)
	{
		z = z * z + c;
		count++;
	}
	return count;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
	if (count == MAX_ITER)
	{
		r = 0;
		g = 0;
		b = 0;
	}
	else
	{
		int region = count / (MAX_ITER / 5);
		int color = count % (MAX_ITER / 5);
		switch (region)
		{
		case 0:
			r = 0;
			g = 0;
			b = 255 - color * 255 / (MAX_ITER / 5);
			break;
		case 1:
			r = 0;
			g = color * 255 / (MAX_ITER / 5);
			b = 255;
			break;
		case 2:
			r = 0;
			g = 255;
			b = 255 - color * 255 / (MAX_ITER / 5);
			break;
		case 3:
			r = color * 255 / (MAX_ITER / 5);
			g = 255;
			b = 0;
			break;
		case 4:
			r = 255;
			g = 255 - color * 255 / (MAX_ITER / 5);
			b = 0;
			break;
		}
	}
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{
	float x = ((mousePixel.x - 0) / (float)m_pixel_size.x) * m_plane_size.x + (m_plane_center.x - m_plane_size.x / 2.0);
	float y = ((mousePixel.y - m_pixel_size.y) / (float)(0 - m_pixel_size.y)) * m_plane_size.y + (m_plane_center.y - m_plane_size.y / 2.0);
	return { x,y };
}



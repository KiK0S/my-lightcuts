// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Image {
public:
	inline Image (size_t width = 64, size_t height = 64) : 
		m_width (width),
		m_height (height) {
		m_pixels.resize (width*height, glm::vec3 (0.f, 0.f, 0.f));
	}

	inline virtual ~Image () {}

	inline size_t width () const { return m_width; }

	inline size_t height () const { return m_height; }

	inline const glm::vec3 & operator() (size_t x, size_t y) const { return m_pixels[y*m_width+x]; }

	inline glm::vec3 & operator() (size_t x, size_t y) { return m_pixels[y*m_width+x]; }

	inline const glm::vec3 & operator[] (size_t i) const { return m_pixels[i]; }

	inline glm::vec3 & operator[] (size_t i) { return m_pixels[i]; }

	inline const std::vector<glm::vec3> & pixels () const { return m_pixels; }

	/// Clear to 'color', black by default.
	inline void clear (const glm::vec3 & color = glm::vec3 (0.f, 0.f, 0.f)) {
		for (size_t y = 0; y < m_height; y++)
			for (size_t x = 0; x < m_width; x++) 
				m_pixels[y*m_width+x] = color;
	}

	inline void savePPM (const std::string & filename) {
		std::ofstream out (filename.c_str ());
    	if (!out) {
        	std::cerr << "Cannot open file " << filename.c_str() << std::endl;
        	std::exit (1);
    	}
    	out << "P3" << std::endl
    		<< m_width << " " << m_height << std::endl
    		<< "255" << std::endl;
    	for (size_t y = 0; y < m_height; y++)
			for (size_t x = 0; x < m_width; x++) {
				out << std::min (255u, static_cast<unsigned int> (255.f * m_pixels[y*m_width+x][0])) << " "
					<< std::min (255u, static_cast<unsigned int> (255.f * m_pixels[y*m_width+x][1])) << " "
					<< std::min (255u, static_cast<unsigned int> (255.f * m_pixels[y*m_width+x][2])) << " ";
			}
			out << std::endl;
    	out.close ();
	}

private:
	size_t m_width;
	size_t m_height;
	std::vector<glm::vec3> m_pixels;
};
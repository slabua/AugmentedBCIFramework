/*****************************************************************************
* 
* UniPA BCI Framework: an Augmented BCI Framework
* 
* Copyright (C) 2016-2017 Salvatore La Bua (slabua@gmail.com)
* RoboticsLab, Department of Industrial and Digital Innovation (DIID),
* Universita' degli Studi di Palermo, V. delle Scienze, Palermo, Italy.
* 
* http://www.slblabs.com/projects/unipa-bci-framework
* https://github.com/slabua/UniPABCIFramework
* 
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
*****************************************************************************/

#include "P300Square.h"

using std::string;

GLuint LoadBMP(const char *);


P300Square::P300Square() { // Costruttore vuoto
	setID(0);
	setU(0.0f);
	setV(0.0f);
	setSize(0.0f);
	setTexturePath("");
	setTextureFlashPath("");
}

P300Square::P300Square(int p_ID) { // costruttore base
	setID(p_ID);
} 

P300Square::P300Square(int p_ID, GLfloat p_u, GLfloat p_v, 
	GLfloat p_size, string p_texturePath, string p_textureFlashPath) { // costruttore completo
	setID(p_ID);
	setU(p_u);
	setV(p_v);
	setSize(p_size);
	setTexturePath(p_texturePath);
	setTextureFlashPath(p_textureFlashPath);
}

P300Square::~P300Square() {} // distruttore vuoto

// ----------------------------------------------------------
// ------------------ Metodi set ----------------------------
// ----------------------------------------------------------

void P300Square::setID(int p_ID) {
	ID = p_ID;
}

void P300Square::setU(GLfloat p_u) {
	u = p_u;
}

void P300Square::setV(GLfloat p_v) {
	v = p_v;
}

void P300Square::setSize(GLfloat p_size) {
	size = p_size;
}

void P300Square::setTexturePath(string p_texturePath) {
	texturePath = p_texturePath;
}

void P300Square::setTextureFlashPath(string p_textureFlashPath) {
	textureFlashPath = p_textureFlashPath;
}

// ----------------------------------------------------------
// ------------------ Metodi get ----------------------------
// ----------------------------------------------------------

int P300Square::getID() {
	return ID;
}

GLfloat P300Square::getU() {
	return u;
}

GLfloat P300Square::getV() {
	return v;
}

GLfloat P300Square::getSize() {
	return size;
}

string P300Square::getTexturePath() {
	return texturePath;
}

string P300Square::getTextureFlashPath() {
	return textureFlashPath;
}

// -----------------------------------------------------
// ------- Metodi per settare le texture ---------------
// -----------------------------------------------------

void P300Square::setTexture(bool func) {
	
	string tmp = "";
	/* SLB use one-liner below instead
	if (func == false)
		tmp = texturePath;
	else
		tmp = textureFlashPath;
	*/
	(func) ? tmp = textureFlashPath : tmp = texturePath; // SLB

	GLuint texture = LoadBMP(tmp.c_str());
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //aumento qualita'
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //aumento luminosita'
	glColor3ub(50, 50, 50);
	
	glEnable(GL_BLEND);
	glBegin(GL_QUADS);
	//glColor4f(0.5f, 0.0, 0.0f, 0.8f); // SLB

	// SLB
	// Bottom Left
	glTexCoord2f(0, 0);
	glVertex2f(u, v);
	// Bottom Right
	glTexCoord2f(1, 0);
	glVertex2f(u+size, v);
	// Top Right
	glTexCoord2f(1, 1);
	glVertex2f(u+size, v+size);
	// Top Left
	glTexCoord2f(0, 1);
	glVertex2f(u, v+size);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

}

GLuint LoadBMP(const char *fileName)
{
    FILE *file;
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int size;
    unsigned int width, height;
    unsigned char *data;

    file = fopen(fileName, "rb");

    if (file == NULL)
    {
        //MessageBox(NULL, L"Error: Invaild file path!", L"Error", MB_OK);
        return false;
    }

    if (fread(header, 1, 54, file) != 54)
    {
        //MessageBox(NULL, L"Error: Invaild file!", L"Error", MB_OK);
        return false;
    }

    if (header[0] != 'B' || header[1] != 'M')
    {
        //MessageBox(NULL, L"Error: Invaild file!", L"Error", MB_OK);
        return false;
    }

    dataPos     = *(int*)&(header[0x0A]);
    size        = *(int*)&(header[0x22]);
    width       = *(int*)&(header[0x12]);
    height      = *(int*)&(header[0x16]);

    if (size == NULL)
        size = width * height * 3;
    if (dataPos == NULL)
        dataPos = 54;

    data = new unsigned char[size];

    fread(data, 1, size, file);
	fclose(file);
    
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	return texture;
}
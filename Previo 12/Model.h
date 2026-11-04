#pragma once

// =====================================================================
//  Model.h - Cargador sencillo de modelos .OBJ con su .MTL y textura.
//  No necesita Assimp ni SOIL2: solo usa stb_image.h para las imagenes.
//
//  Uso:   Model perro((char*)"Models/RedDog.obj");
//         perro.Draw(shader);
//
//  Cada malla se sube a la GPU con el mismo formato que usan los shaders:
//    location 0 = posicion, location 1 = normal, location 2 = coord. textura
//  La textura difusa (map_Kd) se enlaza en la unidad 0 -> "material.diffuse"
// =====================================================================

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstddef>

#include <GL/glew.h>
#include <glm/glm.hpp>

// stb_image: carga JPG/PNG. Model.h solo se incluye en un .cpp, por eso el #define va aqui
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	GLuint textureID;
	GLuint VAO, VBO;

	Mesh(const std::vector<Vertex>& verts, GLuint tex) : vertices(verts), textureID(tex)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
		glBindVertexArray(0);
	}

	void Draw(Shader shader)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(glGetUniformLocation(shader.Program, "material.diffuse"), 0);
		glUniform1i(glGetUniformLocation(shader.Program, "texture_diffuse1"), 0);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
		glBindVertexArray(0);
	}
};

class Model
{
public:
	Model(char* path)
	{
		loadOBJ(path);
	}

	void Draw(Shader shader)
	{
		for (size_t i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::map<std::string, GLuint> loadedTextures;   // para no cargar la misma imagen 2 veces

	// ---------- Carga una imagen y crea la textura de OpenGL ----------
	GLuint loadTexture(const std::string& file)
	{
		if (loadedTextures.count(file)) return loadedTextures[file];

		std::string fullPath = directory + "/" + file;
		GLuint id;
		glGenTextures(1, &id);

		stbi_set_flip_vertically_on_load(true);   // en OBJ la coordenada v=0 esta abajo
		int w, h, channels;
		unsigned char* data = stbi_load(fullPath.c_str(), &w, &h, &channels, 4);  // siempre RGBA
		if (data)
		{
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "No se pudo cargar la textura: " << fullPath << std::endl;
			// textura blanca de 1x1 para que el modelo se vea aunque falte la imagen
			unsigned char white[4] = { 255, 255, 255, 255 };
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		loadedTextures[file] = id;
		return id;
	}

	// ---------- Lee el .mtl: nombre de material -> textura difusa ----------
	void loadMTL(const std::string& file, std::map<std::string, GLuint>& materials)
	{
		std::ifstream in(directory + "/" + file);
		if (!in)
		{
			std::cout << "No se pudo abrir el material: " << directory + "/" + file << std::endl;
			return;
		}
		std::string line, current;
		while (std::getline(in, line))
		{
			std::istringstream ss(line);
			std::string key;
			ss >> key;
			if (key == "newmtl")
			{
				ss >> current;
				materials[current] = 0;
			}
			else if (key == "map_Kd" && !current.empty())
			{
				std::string tex;
				std::getline(ss >> std::ws, tex);
				while (!tex.empty() && (tex.back() == '\r' || tex.back() == ' ')) tex.pop_back();
				materials[current] = loadTexture(tex);
			}
		}
	}

	// Convierte un indice de OBJ (empieza en 1, puede ser negativo) a indice de vector
	static int fixIndex(int idx, size_t count)
	{
		if (idx > 0) return idx - 1;
		if (idx < 0) return (int)count + idx;
		return -1;
	}

	// ---------- Lee el .obj ----------
	void loadOBJ(const std::string& path)
	{
		std::ifstream in(path);
		if (!in)
		{
			std::cout << "No se pudo abrir el modelo: " << path << std::endl;
			return;
		}
		size_t slash = path.find_last_of("/\\");
		directory = (slash == std::string::npos) ? "." : path.substr(0, slash);

		std::vector<glm::vec3> positions, normals;
		std::vector<glm::vec2> uvs;
		std::map<std::string, GLuint> materials;

		std::vector<Vertex> current;
		GLuint currentTex = 0;

		auto flush = [&]()
		{
			if (!current.empty())
			{
				if (currentTex == 0) currentTex = loadTexture("__sin_textura__");
				meshes.push_back(Mesh(current, currentTex));
				current.clear();
			}
		};

		std::string line;
		while (std::getline(in, line))
		{
			std::istringstream ss(line);
			std::string key;
			ss >> key;

			if (key == "v")
			{
				glm::vec3 p; ss >> p.x >> p.y >> p.z; positions.push_back(p);
			}
			else if (key == "vt")
			{
				glm::vec2 t; ss >> t.x >> t.y; uvs.push_back(t);
			}
			else if (key == "vn")
			{
				glm::vec3 n; ss >> n.x >> n.y >> n.z; normals.push_back(n);
			}
			else if (key == "mtllib")
			{
				std::string f; std::getline(ss >> std::ws, f);
				while (!f.empty() && (f.back() == '\r' || f.back() == ' ')) f.pop_back();
				loadMTL(f, materials);
			}
			else if (key == "usemtl")
			{
				flush();
				std::string name; ss >> name;
				currentTex = materials.count(name) ? materials[name] : 0;
			}
			else if (key == "f")
			{
				// Lee todos los vertices de la cara (formato v, v/t, v//n o v/t/n)
				std::vector<Vertex> face;
				std::string token;
				while (ss >> token)
				{
					int vi = 0, ti = 0, ni = 0;
					size_t s1 = token.find('/');
					vi = std::stoi(token.substr(0, s1));
					if (s1 != std::string::npos)
					{
						size_t s2 = token.find('/', s1 + 1);
						std::string t = token.substr(s1 + 1, s2 == std::string::npos ? std::string::npos : s2 - s1 - 1);
						if (!t.empty()) ti = std::stoi(t);
						if (s2 != std::string::npos && s2 + 1 < token.size()) ni = std::stoi(token.substr(s2 + 1));
					}
					Vertex v;
					v.Position  = positions[fixIndex(vi, positions.size())];
					int t = fixIndex(ti, uvs.size());
					int n = fixIndex(ni, normals.size());
					v.TexCoords = (t >= 0) ? uvs[t] : glm::vec2(0.0f);
					v.Normal    = (n >= 0) ? normals[n] : glm::vec3(0.0f, 1.0f, 0.0f);
					face.push_back(v);
				}
				// Triangula (abanico) por si la cara tiene 4 o mas vertices
				for (size_t i = 1; i + 1 < face.size(); i++)
				{
					current.push_back(face[0]);
					current.push_back(face[i]);
					current.push_back(face[i + 1]);
				}
			}
		}
		flush();
	}
};

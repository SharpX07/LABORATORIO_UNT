#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <GameRender/shader.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <GameRender/camera.h>




void drawAxis(Camera camera, Shader simplecolor)
{
	glm::mat4 matrix = glm::mat4(1.0f);
	glm::mat3 view, projection;
	simplecolor.use();
	projection = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 0.01f, 3000.0f);
	view = camera.GetViewMatrix();
	simplecolor.setMat4("projection", matrix);
	simplecolor.setMat4("view", view);
	matrix = glm::translate(matrix, camera.Position);
	matrix = glm::scale(matrix, glm::vec3(0.5, 0.5, 0.5));
	simplecolor.setMat4("model", matrix);
	// light properties
	glLineWidth((GLfloat)2);
	// Habilitar estados fijos
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	simplecolor.setVec3("color", glm::vec3(1, 0, 0));
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0.5f, 0, 0);
	glEnd();
	simplecolor.setVec3("color", glm::vec3(0, 1, 0));
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0.5f, 0);
	glEnd();
	simplecolor.setVec3("color", glm::vec3(0, 0, 1));
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 0.5f);
	glEnd();

}
#include <GameRender/GLDebugDrawer.h>

#include <stdio.h> //printf debugging
#include <glad/glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLDebugDrawer::GLDebugDrawer()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 10000000, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GLDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    float vertices[] = {
        from.x(), from.y(), from.z(),
        to.x(), to.y(), to.z()
    };

    // Assuming MAX_LINES is the maximum number of lines you'll draw in one go
    
    allVertices.insert(allVertices.end(), std::begin(vertices), std::end(vertices));

    // Check if you've reached the maximum lines before flushing the buffer
    if (allVertices.size() / 6 >= 10000000)
    {
        flushLines();
    }
}

void GLDebugDrawer::flushLines()
{
    shaderDebug->use();
    shaderDebug->setMat4("view", camera->GetViewMatrix());
    shaderDebug->setMat4("projection", camera->getProjectionMatrix());

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Transfer all vertices to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * allVertices.size(), allVertices.data());

    // Draw all lines
    glDrawArrays(GL_LINES, 0, allVertices.size() / 3);

    // Clear the buffer for the next set of lines
    allVertices.clear();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void    GLDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

void    GLDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	//glRasterPos3f(location.x(),  location.y(),  location.z());
	//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);
}

void    GLDebugDrawer::reportErrorWarning(const char* warningString)
{
	printf(warningString);
}

void    GLDebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	{
		//btVector3 to=pointOnB+normalOnB*distance;
		//const btVector3&from = pointOnB;
		//glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);   

		//GLDebugDrawer::drawLine(from, to, color);

		//glRasterPos3f(from.x(),  from.y(),  from.z());
		//char buf[12];
		//sprintf(buf," %d",lifeTime);
		//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);
	}
}

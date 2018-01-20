#include <GL/gl.h>
#include <game.h>
#include <math.h>
#include <vector>

using namespace Ob;

void Render::init(int w, int h)
{
	double fw, fh;

	width = w, height = h;
	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	fh = tan(60.0 / 360 * M_PI) * 1.0;
	fw = fh * ((float) w / (float) h);
	glFrustum(-fw, fw, -fh, fh, 1.0, 1024.0);
}

Render::Object Render::addobj(std::vector<float> vert, std::string id)
{
	Object o;
	o.vert = vert;
	o.draw = true;
	o.r = 0, o.g = 0, o.b = 0;
	o.x = 0, o.y = 0, o.z = -5.0;
	o.rotx = 90, o.roty = 0, o.rotz = 150, o.ang = 0;
	obj[id] = o;
	return o;
}

void Render::setobj(std::string id, Object o)
{
	obj[id] = o;
}

Render::Object Render::getobj(std::string id)
{
	return obj[id];
}

void Render::draw()
{
	Object p;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
	for (const auto& i : obj) {
		p = i.second;
		if (!p.draw)
			continue;
		glPushMatrix();
		glColor3f(p.r, p.g, p.b);
		glTranslatef(p.x, p.y, p.z);
		glRotatef(p.rotx, p.roty, p.rotz, p.ang);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &p.vert[0]);
		glDrawArrays(GL_TRIANGLES, 0, p.vert.size() / 3);
		glDisableClientState(GL_VERTEX_ARRAY);
		glPopMatrix();
	}
}

void Render::exit()
{
	return;
}

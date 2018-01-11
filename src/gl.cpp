#include <game.h>
#include <math.h>
#include <GL/gl.h>

using namespace Ob;

void Render::init(int width, int height)
{
	double fw, fh;

	glClearColor(0, 1, 0, 0);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	fh = tan(60.0 / 360 * M_PI) * 1.0;
	fw = fh * ((float) width / (float) height);
	glFrustum(-fw, fw, -fh, fh, 1.0, 1024.0);
}

void Render::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}

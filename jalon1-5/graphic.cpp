#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>

SDL_Renderer* FenetreGraphique_creer (int largeurDeLaFenetre, int hauteurDeLaFenetre, double xMin, 
										double xMax, double yMin, double yMax) {
	SDL_Init (SDL_INIT_VIDEO);
	SDL_Window* displayWindow;
	SDL_RendererInfo displayRendererInfo;
	SDL_Renderer* displayRenderer;
	SDL_CreateWindowAndRenderer (largeurDeLaFenetre, hauteurDeLaFenetre, SDL_WINDOW_OPENGL,
	&displayWindow, &displayRenderer);
	SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
	glViewport (0, 0, (GLsizei) largeurDeLaFenetre, (GLsizei) hauteurDeLaFenetre);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glOrtho (xMin, xMax, yMin, yMax, 1, -1);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	return displayRenderer;
} 

void FenetreGraphique_initialiser () {
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FenetreGraphique_rendre (SDL_Renderer* displayRenderer) {
	SDL_RenderPresent (displayRenderer);
	SDL_Delay(5000);
}

void FenetreGraphique_fermer () {
	SDL_Quit();
}

void FenetreGrahique_dessinerRectangle (double x1, double y1, double x2, double y2,
										float fondR, float fondV, float fondB,
										float formeR, float formeV, float formeB) {
	glColor3f (fondR, fondV ,fondB);

	glBegin (GL_QUADS);
		glVertex2f (x1, y1);
		glVertex2f (x2, y1);
		glVertex2f (x2, y2);
		glVertex2f (x1, y2);
	glEnd ();

	glColor3f (formeR, formeV ,formeB);

	glBegin (GL_LINE_LOOP);
		glVertex2f (x1, y1);
		glVertex2f (x2, y1);
		glVertex2f (x2, y2);
		glVertex2f (x1, y2);
	glEnd ();
}

int main(int argc, char *argv[]) {
	SDL_Renderer* displayRenderer = FenetreGraphique_creer (800, 600, -2, 2, -2, 2);
	FenetreGraphique_initialiser ();
	FenetreGrahique_dessinerRectangle (-0.8, -0.8, 0.8, 0.8, 1, 0, 1, 0, 1, 0);
	FenetreGraphique_rendre (displayRenderer);
	FenetreGraphique_fermer ();
	return 0;
} 
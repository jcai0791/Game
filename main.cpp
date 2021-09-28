#include <iostream>
#include <string.h>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <SDL.h>

using namespace std;

/*
 *
 *	Hello World in SDL2.
 *
 */

/* Global Constants */
const int SECOND = 1000;
const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH = 640;
const double FOV_HORIZONTAL = 2.0944;
const double FOV_VERTICAL = 1.5708;
//Variables


// init ... The init function, it calls the SDL init function.
int init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		return -1;
	}

	return 0;
}

// createWindowContext ... Creating the window for later use in rendering and stuff.
SDL_Window *createWindowContext(std::string title)
{
	//Declaring the variable the return later.
	SDL_Window *Window = NULL;

	//Creating the window and passing that reference to the previously declared variable.
	Window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_UpdateWindowSurface(Window);

	//Returning the newly creted Window context.
	return Window;
}

// createRenderContext ... Creating the rendering context so that I can render things to the Window.
SDL_Renderer *createRendererContext(SDL_Window *Window)
{
	//Declaring the variable for the Renderer context.
	SDL_Renderer *Renderer = NULL;

	//Create the Renderer with the window.
	Renderer = SDL_CreateRenderer(Window, -1, 0);

	//Return the Renderer.
	return Renderer;
}

//Simplify drawing line
void draw_line(SDL_Renderer *Renderer, pair<int, int> point1, pair<int, int> point2){
	SDL_RenderDrawLine(Renderer, point1.first, point1.second, point2.first, point2.second);
}

double dot_product(vector<double> v1, vector<double> v2){
	double total = 0;
	for(int i = 0; i<min(v1.size(), v2.size()); i++) total+=v1[i]*v2[i];
	return total;
}

vector<double> unit_vector(double thetaH, double thetaV){
	double unitVectorX = cos(thetaV)*cos(thetaH);
	double unitVectorY = cos(thetaV)*sin(thetaH);
	double unitVectorZ = sin(thetaV);
	vector<double> v = {unitVectorX, unitVectorY, unitVectorZ};
	return v;
}

// draw ... Draw a series of lines to the screen to create a triangle. Tint is between 0 and 1 and specifies how dark it is.
void draw_triangle(SDL_Renderer *Renderer, vector<pair<int, int> > points, double tint)
{
	//Set the draw color...
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	//Clear the screen.
	SDL_RenderClear(Renderer);

	//Setting the actual draw color.
	SDL_SetRenderDrawColor(Renderer, 150*tint, 150*tint, 250*tint, SDL_ALPHA_OPAQUE);

	//Drawing lines between the points
	for (int i = 0; i < points.size()-1; i++)
	{
		SDL_RenderDrawLine(Renderer, points[i].first, points[i].second, points[i+1].first, points[i+1].second);
	}
	SDL_RenderDrawLine(Renderer, points[points.size()-1].first, points[points.size()-1].second, points[0].first, points[0].second);

	//Filling it in
	sort(points.begin(), points.end());
	pair<int , int> high = points[2];
	pair<int , int> mid = points[1];
	pair<int , int> low = points[0];
	//Calculating intersection of mid line with other side
	pair<int , int> midpoint = make_pair(mid.first,(int)((static_cast<double>(mid.first-low.first)/(static_cast<double>(high.first-low.first)))*(high.second-low.second)+low.second));

	//Filling it in
	for(int i = 0; i<mid.first-low.first; i++){
		double ratio = i/static_cast<double>(mid.first-low.first);
		SDL_RenderDrawLine(Renderer, i+low.first, (midpoint.second-low.second)*ratio+low.second, i+low.first, (mid.second-low.second)*ratio+low.second);
	}
	for(int i = 0; i<high.first-mid.first; i++){
		double ratio = i/static_cast<double>(high.first-mid.first);
		SDL_RenderDrawLine(Renderer, i+mid.first, (high.second-mid.second)*ratio+mid.second, i+mid.first, (high.second-midpoint.second)*ratio+midpoint.second);
	}

	//Update the Renderer.
	SDL_RenderPresent(Renderer);
}

//Convert to 2d. Returns null if point should not be displayed.
pair<int, int> flatten(double thetaH, double thetaV, vector<double> observerPosition, vector<double> pointPosition){
	double x = pointPosition[0]-observerPosition[0];
	double y = pointPosition[1]-observerPosition[1];
	double z = pointPosition[2]-observerPosition[2];
	vector<double> view = {x,y,z};
	pair<int, int> invalid = {-1, -1};
	if(dot_product(unit_vector(thetaH, thetaV),view)<0) return invalid;

	//Possible bug: Divide by 0 errors and all that. Only a problem if the positions are exactly the same.
	double omegaH = atan(y/x);
	double omegaV = atan(z/sqrt(x*x+y*y));
	double angleH = thetaH - omegaH;
	double angleV = thetaV - omegaV;
	//if(2*abs(angleH)>FOV_HORIZONTAL) return invalid;
	//if(2*abs(angleV)>FOV_VERTICAL) return invalid;

	double distance = static_cast<double>(SCREEN_WIDTH)/(2*tan(FOV_HORIZONTAL/2.0));
	int deviationHorizontal = (int)(distance*tan(angleH));
	int deviationVertical = (int)(distance*tan(angleV));
	pair<int, int> finalPoint = {SCREEN_WIDTH/2+deviationHorizontal, SCREEN_HEIGHT/2+deviationVertical};
	return finalPoint;
}

void render_triangle(SDL_Renderer *Renderer, double thetaH, double thetaV, vector<double> observerPosition, vector<vector<double> > points){
	vector<double> point1 = points[0];
	vector<double> point2 = points[1];
	vector<double> point3 = points[2];

	pair<int, int> flattened1 = flatten(thetaH, thetaV, observerPosition, point1);
	pair<int, int> flattened2 = flatten(thetaH, thetaV, observerPosition, point2);
	pair<int, int> flattened3 = flatten(thetaH, thetaV, observerPosition, point3);
	vector< pair <int, int> > flattened = {flattened1, flattened2, flattened3};
	//Drawing!
	draw_triangle(Renderer, flattened, 1.0);
}

// main ... The main function, right now it just calls the initialization of SDL.
int main(int argc, char *argv[])
{
	//Calling the SDL init stuff.
	init();

	//Creating the context for SDL2.
	SDL_Window *Window = createWindowContext("Hello World!");

	//Creating the rendering context.
	SDL_Renderer *Renderer = createRendererContext(Window);

	vector<vector<double> > points = {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}};

	vector<double> observer = {-0.5, -0.0, -0.5};
	double angle1 = 1.0;
	double angle2 = 1.0;
	render_triangle(Renderer, angle1, angle2, observer, points);

	SDL_UpdateWindowSurface(Window);
	//Adding a delay.
	SDL_Event e;
	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			if (e.type == SDL_KEYDOWN)
			{
				if(e.key.keysym.sym == SDLK_LEFT){
					angle1-=0.05;
					render_triangle(Renderer, angle1, angle2, observer, points);
				}
				if(e.key.keysym.sym == SDLK_RIGHT){
					angle1+=0.05;
					render_triangle(Renderer, angle1, angle2, observer, points);
				}
				if(e.key.keysym.sym == SDLK_UP){
					angle2+=0.05;
					render_triangle(Renderer, angle1, angle2, observer, points);
				}
				if(e.key.keysym.sym == SDLK_DOWN){
					angle2-=0.05;
					render_triangle(Renderer, angle1, angle2, observer, points);
				}
				if(e.key.keysym.sym == SDLK_w){
					observer[0]+=0.1*cos(angle1);
					observer[1]+=0.1*sin(angle1);
					render_triangle(Renderer, angle1, angle2, observer, points);
				}
				if(e.key.keysym.sym == SDLK_a){
					observer[0]+=0.1*sin(angle1);
					observer[1]-=0.1*cos(angle1);
					angle2-=0.05;
					render_triangle(Renderer, angle1, angle2, observer, points);
				}
				if(e.key.keysym.sym == SDLK_s){
					observer[0]-=0.1*cos(angle1);
					observer[1]+=0.1*sin(angle1);
					angle2-=0.05;
					render_triangle(Renderer, angle1, angle2, observer, points);
				}
				if(e.key.keysym.sym == SDLK_d){
					observer[0]+=0.1*sin(angle1);
					observer[1]+=0.1*cos(angle1);
					angle2-=0.05;
					render_triangle(Renderer, angle1, angle2, observer, points);
				}
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				quit = true;
			}
		}
	}

	//Just printing out hello world if everything is working properly.
	std::cout << "Hello World!" << std::endl;

	//Cleaning up everything.
	SDL_Quit();
	return 0;
}
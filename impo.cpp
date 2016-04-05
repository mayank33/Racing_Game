


//
gluLookAt(
			-3.0+bkx-1.0,      0.2,     -3.0+bky-1.0,
			-3.0+bkx+cos(DEG2RAD(-angl)),      0.1,         -3.0+bky+sin(DEG2RAD(-angl)),
			0.0,    1.0,    0.0);

gluLookAt(
			-3.0+bkx,      0.2,     -3.0+bky,
			-3.0+bkx+cos(DEG2RAD(-angl)),      0.1,         -3.0+bky+sin(DEG2RAD(-angl)),
			0.0,    1.0,    0.0);

//Bike view
gluLookAt(
			-3.0+bkx,      0.5,     -3.0+bky,
			-3.0+bkx+cos(DEG2RAD(-angl)),      0.5,         -3.0+bky+sin(DEG2RAD(-angl)),
			0.0,    1.0,    0.0);

//Overhead view
gluLookAt(
			-3.0+bkx,      3.0,     -3.0+bky,
			-3.0+bkx+cos(DEG2RAD(-angl)),      0.0,         -3.0+bky+sin(DEG2RAD(-angl)),
			0.0,    1.0,    0.0);


//final driver view
gluLookAt(
			bik_x-10*cos(DEG2RAD(-angl)),height+30,bik_z-10*sin(DEG2RAD(-angl)),
			bik_x+30*cos(DEG2RAD(-angl)),height+10,bik_z+30*sin(DEG2RAD(-angl)),
			0.0,    1.0,    0.0);


//final overhead view
gluLookAt(
			bik_x-40*cos(DEG2RAD(-angl)),height+30,bik_z-40*sin(DEG2RAD(-angl)),
			bik_x+10*cos(DEG2RAD(-angl)),height+10,bik_z+10*sin(DEG2RAD(-angl)),
			0.0,    1.0,    0.0);

//final wheel view
gluLookAt(
			bik_x+10*cos(DEG2RAD(-angl)),height+6,bik_z+10*sin(DEG2RAD(-angl)),
			bik_x+15*cos(DEG2RAD(-angl)),height+4,bik_z+15*sin(DEG2RAD(-angl)),
			0.0,    1.0,    0.0);
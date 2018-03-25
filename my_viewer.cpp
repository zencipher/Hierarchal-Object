# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ws_run.h>


GsVec light_vec(0, 200.0f, 50.0f);
bool animating = false;

MyViewer::MyViewer ( int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)
{
	
	_nbut=0;
	_animating=false;
	build_ui ();
	build_scene ();
}

 GsMat add_shadow() //using function from PA3
{
	GsMat shadow;
	GsMat rotate;
	shadow.setl1(1, 0, -light_vec.x/light_vec.z , 0);
	shadow.setl2(0, 1, -light_vec.y/light_vec.z, 0);
	shadow.setl3(0, 0, 0, 0);
	shadow.setl4(0, 0, 0, 1);

	shadow.setrans(0, 20, 0);
	//shadow.mult(shadow, rotate);
	
	return shadow;
}

void MyViewer::build_ui ()
{
	UiPanel *p, *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	p->add ( new UiButton ( "View", sp=new UiPanel() ) );
	{	UiPanel* p=sp;
		p->add ( _nbut=new UiCheckButton ( "Normals", EvNormals ) ); 
	}
	p->add ( new UiButton ( "Animate", EvAnimate ) );
	p->add ( new UiButton ( "Exit", EvExit ) ); p->top()->separate();
}

void MyViewer::add_model ( SnShape* s, GsVec p )
{
	SnManipulator* manip = new SnManipulator;
	GsMat m;
	m.translation ( p );
	manip->initial_mat ( m );

	SnGroup* g = new SnGroup;
	SnLines* l = new SnLines;
	l->color(GsColor::orange);
	g->add(s);
	g->add(l);
	manip->child(g);

	rootg()->add(manip);
}

void MyViewer::moveCamera() {
	GsVec location2;
	GsVec location3;
	GsMat location = manip_main_drill->mat();
	location.getrans(location2);
	location2.y += 300;
	location2.x += 10;

	camera().eye.x = location2.x;
	camera().eye.y = location2.y;
	camera().eye.z = location2.z;

	location.getrans(location3);
	camera().center.x = location3.x;
	camera().center.y = location3.y;
	camera().center.z = location3.z;

	render();
	ws_check();
}


void MyViewer::build_scene ()
{
	SnGroup* terrain_group = new SnGroup;
	SnGroup* drill_group = new SnGroup; //group for ENTIRE drill

	GsMat newShadow = add_shadow();
	GsMat baseManip = manip_main_drill->mat();

	newShadow.mult(newShadow, baseManip);
	shadow_manip->initial_mat(newShadow);
	//drill_group->add(shadow_manip);

	SnGroup* newgroup = new SnGroup;

	newgroup->add(drill_group);
	newgroup->add(shadow_manip);
	
	GsMat m;
	GsMat rotx;
	rotx.rotx(-3.1415f/2);
	m.translation(GsVec(0, 0, 0));
	m.mult(m, rotx);
	manip_main_drill->initial_mat(m);

	/*GsMat rotbx;
	rotx.rotx(3.1415f / 2);
	shadow_manip.translation(GsVec(0, 0, 0));
	shadow_manip.mult(shadow_manip, rotbx);
	shadow_manip->initial_mat(s);*/

	GsMat t;
	t.translation(GsVec(0, -10, -10));
	t.mult(t, rotx);
	terrain_manip->initial_mat(t);

	SnModel* terrain = new SnModel;
	terrain->model()->load("C:\\Users\\Veronica\\Documents\\CSE170\\Project1\\terrain\\tinker.obj");
	terrain->model()->smooth();

	terrain_manip->child(terrain_group);
	terrain_group->add(terrain);

	SnModel* drill = new SnModel; //Belongs to drill_group and manip_main_drill ONLY; also refers to the drill BODY
	drill->model()->load("C:\\Users\\Veronica\\Documents\\CSE170\\Project1\\mainobj\\tinker.obj");
	drill->model()->smooth();

	
	//SnGroup* drillParts_group = new SnGroup; //group for moving parts of drill
	SnModel* drillHead = new SnModel; //the rotating drill
	drillHead->model()->load("C:\\Users\\Veronica\\Documents\\CSE170\\Project1\\DrillBar\\tinker.obj");
	drillHead->model()->smooth();

	manip_main_drill->child(newgroup);
	drill_group->add(drill);

	drillHead_manip->child(drillHead);
	
	//Position the drillhead to the front of the drill body
	GsMat move_drillhead;
	move_drillhead.translation(GsVec(0, -50, 30));
	drillHead_manip->initial_mat(move_drillhead);

	SnModel* propel = new SnModel;
	propel->model()->load("C:\\Users\\Veronica\\Documents\\CSE170\\Project1\\propellar\\tinker.obj");
	propel->model()->smooth();

	drill_group->add(propel_manip);
	propel_manip->child(propel);

	GsMat move_propel;
	move_propel.translation(GsVec(0, 0, 80));
	propel_manip->initial_mat(move_propel);

	SnModel* knife = new SnModel;
	knife->model()->load("C:\\Users\\Veronica\\Documents\\CSE170\\Project1\\knife\\tinker.obj");
	knife->model()->smooth();

	drill_group->add(knife_manip);
	knife_manip->child(knife);

	GsMat move_knife;
	move_knife.translation(GsVec(0, 40, 40));
	knife_manip->initial_mat(move_knife);


	SnGroup* wheel_group = new SnGroup;
	SnModel* wheelFR = new SnModel; //Front right wheel
	wheelFR->model()->load("C:\\Users\\Veronica\\Documents\\CSE170\\Project1\\FrontRightWheel\\tinker.obj");
	wheelFR->model()->smooth();
	
	drill_group->add(wheel_group);
	wheel_group->add(wheel1_manip);
	wheel1_manip->child(wheelFR);

	//position the front right wheel
	GsMat move_FRWheel;
	move_FRWheel.translation(GsVec(-20, -30, 0));
	wheel1_manip->initial_mat(move_FRWheel);

	SnModel* wheelFL = new SnModel; //Front left
	wheelFL->model()->load("C:\\Users\\Veronica\\Documents\\CSE170\\Project1\\FrontLeftWheel\\tinker.obj");
	wheelFL->model()->smooth();
	
	wheel_group->add(wheel2_manip);
	wheel2_manip->child(wheelFL);

	GsMat move_FLWheel;
	move_FLWheel.translation(GsVec(20, -30, 0));
	wheel2_manip->initial_mat(move_FLWheel);

	SnModel* wheelBR = new SnModel; //Back right
	wheelBR->model()->load("C:\\Users\\Veronica\\Documents\\CSE170\\Project1\\BackRightWheel\\tinker.obj");
	wheelBR->model()->smooth();
	
	wheel_group->add(wheel3_manip);
	wheel3_manip->child(wheelBR);

	GsMat move_BRWheel;
	move_BRWheel.translation(GsVec(-20, 25, 0));
	wheel3_manip->initial_mat(move_BRWheel);

	SnModel* wheelBL = new SnModel; //Back left
	wheelBL->model()->load("C:\\Users\\Veronica\\Documents\\CSE170\\Project1\\BackLeftWheel\\tinker.obj");
	wheelBL->model()->smooth();

	wheel_group->add(wheel4_manip);
	wheel4_manip->child(wheelBL);

	GsMat move_BLWheel;
	move_BLWheel.translation(GsVec(20, 25, 0));
	wheel4_manip->initial_mat(move_BLWheel);
	
	drill_group->add(drillHead_manip); //add the drill head manipulator to the MAIN group
	

	//GsInput x;
	
	//rootg()->add(manip_main_drill); //load in the full drill
	

	shadow_manip->child(drill_group);
	//drill_group->add(shadow_manip);

	
	
	rootg()->add(manip_main_drill);
	rootg()->add(terrain_manip);
	//rootg()->add(shadow_manip);
	
	//add_shadow(manip_main_drill, GsVec(0, 0, 0));
	
}


float dmoveZ = -0.7f;
float dangle = 0.01f;

void MyViewer::move(int type) {
		//SnManipulator* manip = manip_main_drill; // access one of the manipulators
		SnManipulator* drillManip = drillHead_manip;
		
		GsMat m, dm, wm1, wm2, wm3, wm4, pm, km;
		GsMat a, b, c, d, e, f;
		GsMat rotx, rotDrillBar;

		wm1 = wheel1_manip->mat();
		wm2 = wheel2_manip->mat();
		wm3 = wheel3_manip->mat();
		wm4 = wheel4_manip->mat();

		if (type == 0) { //move W -> move forward
			m = manip_main_drill->mat();
			a.translation(GsVec(0, dmoveZ,0));

			c.rotx(0.05f);
			wm1.mult(wm1, c);
			wm2.mult(wm2, c);
			wm3.mult(wm3, c);
			wm4.mult(wm4, c);
			
			wheel1_manip->initial_mat(wm1);
			wheel2_manip->initial_mat(wm2);
			wheel3_manip->initial_mat(wm3);
			wheel4_manip->initial_mat(wm4);

			m.mult(m,a);
			
			manip_main_drill->initial_mat(m);

			moveCamera();
			
			render(); // notify it needs redraw
			ws_check(); // redraw now
		}
		 
		if (type == 1) { //move S -> move back
			m = manip_main_drill->mat();
			a.translation(GsVec(0, -dmoveZ, 0));

			c.rotx(-0.05f);
			wm1.mult(wm1, c);
			wm2.mult(wm2, c);
			wm3.mult(wm3, c);
			wm4.mult(wm4, c);

			wheel1_manip->initial_mat(wm1);
			wheel2_manip->initial_mat(wm2);
			wheel3_manip->initial_mat(wm3);
			wheel4_manip->initial_mat(wm4);
			
			m.mult(m, a);
		
			manip_main_drill->initial_mat(m);
			moveCamera();

			render(); // notify it needs redraw
			ws_check(); // redraw now
		}

		if (type == 2) { //move A -> rotate drill body left
			m = manip_main_drill->mat();
			GsMat rotation;
			rotation.rotz(0.02f);
			m.mult(m, rotation);
			manip_main_drill->initial_mat(m);
			moveCamera();

			render(); // notify it needs redraw
			ws_check(); // redraw now
		}

		if (type == 3) { //turn D -> rotate drill body right
			m = manip_main_drill->mat();
			GsMat rotation;
			rotation.rotz(-0.02f);
			m.mult(m, rotation);
			manip_main_drill->initial_mat(m);
			moveCamera();

			render(); // notify it needs redraw
			ws_check(); // redraw now
		}

		if (type == 4) { //turn Q -> move drill upward
			m = manip_main_drill->mat();
			a.translation(GsVec(0,0, -dmoveZ));

			pm = propel_manip->mat();
			d.rotz(0.08f);
			pm.mult(pm, d);
			propel_manip->initial_mat(pm);

			m.mult(m, a);

			manip_main_drill->initial_mat(m);
			moveCamera();

			render(); // notify it needs redraw
			ws_check(); // redraw now
		}

		if (type == 5) { //turn E -> move drill upward
			m = manip_main_drill->mat();
			a.translation(GsVec(0, 0, dmoveZ));


			m.mult(m, a);

			manip_main_drill->initial_mat(m);
			moveCamera();

			render(); // notify it needs redraw
			ws_check(); // redraw now
		}	
		
		if (type == 6) {
			km = knife_manip->mat();
			f.rotz(0.2f);
			km.mult(km, f);
			knife_manip->initial_mat(km);
			render();
			ws_check();
		}

		if (type == 7) {
			km = knife_manip->mat();
			f.rotz(-0.2f);
			km.mult(km, f);
			knife_manip->initial_mat(km);
			render();
			ws_check();
		}

		animating = false; //for when spinning the wheels (LATER)

		while (!animating) { //continuously spin the drill head
			pm = propel_manip->mat();
			d.rotz(dangle);
			dm = drillManip->mat();
			b.roty(dangle);
			pm.mult(pm, d);
			propel_manip->initial_mat(pm);
			dm.mult(dm, b);
			drillManip->initial_mat(dm);
			render(); // notify it needs redraw
			ws_check(); // redraw now
		}

}

// Below is an example of how to control the main loop of an animation:
void MyViewer::run_animation ()
{
	if ( _animating ) return; // avoid recursive calls
	_animating = true;
	
	int ind = gs_random ( 0, rootg()->size()-1 ); // pick one child
	SnManipulator* manip = rootg()->get<SnManipulator>(ind); // access one of the manipulators
	GsMat m = manip->mat();

	double frdt = 1.0/30.0; // delta time to reach given number of frames per second
	double v = 4; // target velocity is 1 unit per second
	double t=0, lt=0, t0=gs_time();
	do // run for a while:
	{	while ( t-lt<frdt ) { ws_check(); t=gs_time()-t0; } // wait until it is time for next frame
		double yinc = (t-lt)*v;
		if ( t>2 ) yinc=-yinc; // after 2 secs: go down
		lt = t;
		m.e24 += (float)yinc;
		if ( m.e24<0 ) m.e24=0; // make sure it does not go below 0
		manip->initial_mat ( m );
		render(); // notify it needs redraw
		ws_check(); // redraw now
	}	while ( m.e24>0 );
	_animating = false;
}

void MyViewer::show_normals ( bool b )
{
	// Note that primitives are only converted to meshes in GsModel
	// at the first draw call.
	GsArray<GsVec> fn;
	SnGroup* r = (SnGroup*)root();
	for ( int k=0; k<r->size(); k++ )
	{	SnManipulator* manip = r->get<SnManipulator>(k);
		SnShape* s = manip->child<SnGroup>()->get<SnShape>(0);
		SnLines* l = manip->child<SnGroup>()->get<SnLines>(1);
		if ( !b ) { l->visible(false); continue; }
		l->visible ( true );
		if ( !l->empty() ) continue; // build only once
		l->init();
		if ( s->instance_name()==SnPrimitive::class_name )
		{	GsModel& m = *((SnModel*)s)->model();
			m.get_normals_per_face ( fn );
			const GsVec* n = fn.pt();
			float f = 0.33f;
			for ( int i=0; i<m.F.size(); i++ )
			{	const GsVec& a=m.V[m.F[i].a]; l->push ( a, a+(*n++)*f );
				const GsVec& b=m.V[m.F[i].b]; l->push ( b, b+(*n++)*f );
				const GsVec& c=m.V[m.F[i].c]; l->push ( c, c+(*n++)*f );
			}
		}  
	}
}

int MyViewer::handle_keyboard ( const GsEvent &e )
{
	int ret = WsViewer::handle_keyboard ( e ); // 1st let system check events
	if ( ret ) return ret;

	switch ( e.key )
	{	case GsEvent::KeyEsc : gs_exit(); return 1;
		case 'n' : { bool b=!_nbut->value(); _nbut->value(b); show_normals(b); return 1; }

		case 'w': {
			animating = true;
			move(0); 
			return 1; 
		}
		case 's': {
			animating = true;
			move(1);
			return 1;
		}
		case 'a': {
			animating = true;
			move(2);
			return 1;
		}

		case 'd': {
			animating = true;
			move(3);
			return 1;
		}

		case 'q': {
			animating = true;
			move(4);
			return 1;
		}

		case 'e': {
			animating = true;
			move(5);
			return 1;
		}

		case 'z': {
			animating = true;
			move(6);
			return 1;
		}

		case 'x': {
			animating = true;
			move(7);
			return 1;
		}

		case ' ': {
			//camera().eye
			GsVec location2;
			GsVec location3;
			GsMat location = manip_main_drill->mat();
			location.getrans(location2);
			location2.y += 300;
			location2.x += 10;

			camera().eye.x = location2.x;
			camera().eye.y = location2.y;
			camera().eye.z = location2.z;

			location.getrans(location3);
			camera().center.x = location3.x;
			camera().center.y = location3.y;
			camera().center.z = location3.z;

			render();
			ws_check();

			return 1;
		}

		default: gsout<<"Key pressed: "<<e.key<<gsnl;
	}

	return 0;
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{	case EvNormals: show_normals(_nbut->value()); return 1;
		case EvAnimate: run_animation(); return 1;
		case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}

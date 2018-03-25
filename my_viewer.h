# pragma once

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>

# include <sigogl/ui_button.h>
# include <sigogl/ws_viewer.h>
# include <sig/sn_manipulator.h>

// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	enum MenuEv { EvNormals, EvAnimate, EvExit };
	UiCheckButton* _nbut;
	bool _animating;
   public :
	   SnManipulator * terrain_manip = new SnManipulator;
	   SnManipulator * manip_main_drill = new SnManipulator; //Manipulates ENTIRE drill
	   SnManipulator* drillHead_manip = new SnManipulator; //Manipulates the rotating drill
	   SnManipulator* propel_manip = new SnManipulator; //manipulates propeller
	   SnManipulator* knife_manip = new SnManipulator;
	   SnManipulator* wheel1_manip = new SnManipulator;
	   SnManipulator* wheel2_manip = new SnManipulator;
	   SnManipulator* wheel3_manip = new SnManipulator;
	   SnManipulator* wheel4_manip = new SnManipulator;

	   SnManipulator* shadow_manip = new SnManipulator;
	MyViewer ( int x, int y, int w, int h, const char* l );

	void build_ui ();
	void moveCamera();
	void add_model ( SnShape* s, GsVec p );
	void build_scene ();
	void show_normals ( bool b );
	void run_animation ();
	void move(int type);
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int uievent ( int e ) override;
};


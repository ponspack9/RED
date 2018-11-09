#ifndef __j1APP_H__
#define __j1APP_H__

#include "p2List.h"
#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "PugiXml\src\pugixml.hpp"

// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Audio;
class j1Scene;
class j1Map;
class j1Debug;
class j1Collision;
class j1FadeToBlack;
class j1Player;
class j1Enemies;

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(j1Module* module);

	// Exposing some properties for reading
	int			GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame();
	void SaveGame() const;
	//TO BE IMPLEMENTED
	void GetSaveGames(p2List<p2SString>& list_to_fill) const;

	//Load XML, LOGs the result also returning it
	pugi::xml_parse_result LoadXML(pugi::xml_document & doc, const char* path);

	//Restarts the game from the very first level
	bool RestartGame();

	//Restarts the current level
	bool RestartLevel();

	bool SoftRestartLevel();

	bool NextLevel();

private:

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	//Saving iteration ONLY SAVING ACTIVE MODULES
	bool SaveGameFile();

	//Loading iteration ONLY LOADING ACTIVE MODULES
	bool LoadGameFile();

public:

	// Modules
	j1Window*			win;
	j1Input*			input;
	j1Render*			render;
	j1Textures*			tex;
	j1Audio*			audio;
	j1Scene*			scene;
	j1Map*				map;
	j1Debug*			debug;
	j1Collision*		collision;
	j1FadeToBlack*		fade;
	j1Player*			player;
	j1Enemies*			enemies;


	bool				want_to_load;
	mutable bool		want_to_save;

private:

	p2List<j1Module*>	modules;
	float				dt;
	int					argc;
	char**				args;

	p2SString			title;
	p2SString			organization;

	p2SString			load_path;
	mutable p2SString	save_path;
	
	//Framerate control
	j1Timer				timer;
	j1Timer				aux_timer; //to control the amount of frames in the last second
	j1Timer				frame_time;

	j1PerfTimer			perf_timer;

	uint64				frame_count = 0;
	uint32				frames_on_last_update = 0;
	uint32				aux_frames_counter = 0; //to control the amount of frames in the last second
	uint32				last_sec_fcount = 0;
	uint				framerate_cap;

};

extern j1App* App;

#endif
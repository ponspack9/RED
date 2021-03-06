#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"

#define DEFAULT_MUSIC_FADE_TIME 1.5f

struct _Mix_Music;
struct Mix_Chunk;

enum FXtype
{
	click = 1,
	sjump,
	djump,
	airsmash,
	die,
	dieEnemy,
	lifeup,
	coin,
	save
};

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	void ToggleMusicPause();

public:

	//TODO : see module map for reference
	p2SString			folder_music;
	p2SString			folder_fx;
	p2List<p2SString>	tracks_path;
	p2List<p2SString>	fx_path;

	int					current_volume;
	
private:

	_Mix_Music*			music;
	p2List<Mix_Chunk*>	fx;
};

#endif // __j1AUDIO_H__
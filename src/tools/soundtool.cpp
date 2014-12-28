/*
 *  soundtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */
#include "soundtool.h"

#include <cmath>
#include <cstdio>
#include <sstream>
#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "restypes.hpp"
#include "mathutil.h"

std::shared_ptr<sf::SoundBuffer> sound_handles[NUM_SOUNDS];
sf::Sound chan[4];
const char numchannel = 4;
char channel;
short snd_played[4];

bool play_sounds = true;
short last_played;
std::unordered_set<int> always_async = {
	6,24,25,34,37,
	39,41,42,43,44,
	45,46,47,48,49,
	50,55,61,76,77,
	78,79,80,81,82,
	83,85,91
};
std::unordered_set<int> load_on_init = {
	0,1,6,10,11,
	12,14,27,34,37,
	47,48,49,50,55,
	61,69,70,71,72,
	73,74,75,85,86,
	87,88,89,99
};
std::unordered_map<int,int> sound_delay = {
	{24,25},{25,25},{34,8},{37,8},{43,10},
	{44,20},{61,13}
};
short store_last_sound_played;

static bool sound_going(snd_num_t which_s) {
	short i;
	
	for(i = 0; i < 4; i++)
		if(snd_played[i] == which_s)
			return true;
	return false;
}

static std::string sound_to_fname_map(snd_num_t snd_num) {
	std::ostringstream sout;
	sout << "SND" << snd_num;
	return sout.str();
}

void init_snd_tool(){
	ResMgr::setIdMapFn<SoundRsrc>(sound_to_fname_map);
	
	// TODO: Might need sound 0, not sure
	for(int i : load_on_init) {
		if(i == 0) continue;
		sound_handles[i] = ResMgr::get<SoundRsrc>(i);
	}
}

void play_sound(short which, short how_many_times) { // if < 0, play asynch
	static bool inited = false;
	if(!inited) {
		inited = true;
		ResMgr::setIdMapFn<SoundRsrc>(sound_to_fname_map);
	}

	std::shared_ptr<sf::SoundBuffer> sndhandle;
	if(!play_sounds || how_many_times == 0) return;
	
	if(abs(which) > NUM_SOUNDS) {
		//char msg[50];
		/*s*/printf(/*msg,*/"Error: Sound #%i does not exist.\n",abs(which));
		//give_error(msg,"",0);
		return;
	}
	
	channel++;
	
	if(channel >= numchannel) channel = 0;
	
	if(!sound_going(abs(which)) && load_on_init.find(abs(which)) == load_on_init.end())
		sndhandle = ResMgr::get<SoundRsrc>(abs(which));
	else sndhandle = sound_handles[abs(which)];
	
	if(which > 0)
 		if(always_async.find(which) != always_async.end())
			which *= -1;
	
 	if(sndhandle) {
		chan[channel] = sf::Sound(*sndhandle);
		chan[channel].play();
		
		if(which > 0) {
			while(chan[channel].getStatus() == sf::Sound::Playing);
		}
		snd_played[channel] = abs(which);
	}
	if(which < 0)
		sf::sleep(time_in_ticks(sound_delay[-1 * which]));
	if(how_many_times > 1)
		play_sound(which, how_many_times - 1);
	// TODO: Since sounds are handled by SFML, all this Windows-specific code shouldn't be needed
#if defined(WIN32)
	short i,num_fails = 0;
	char snd_name[30];
	bool asyn = false,a_sound_did_get_played = false;
	bool not_asyn = false,check_sound;
	HRSRC h;
	if((sounds_missing) || (!play_sounds) || (how_many_times == 0))
		return;
	
	if(which < 0) {
		asyn = true;
		which = which * -1;
		}
	if(which >= 1000) {
		which -= 1000;
		not_asyn = true;
		}

	if(which >= 100)
		return;

	if((always_asynch[which]) &&
	((can_ignore[which] == 1) || (can_ignore[which] >= 3)))
		asyn = true;
	if((can_ignore[which] > 0) && (can_ignore[which] < 5) && (party.stuff_done[305][5] == 1))
		return;
	if((can_ignore[which] != 1) && (can_ignore[which] < 3))
		asyn = false;
	if((party.stuff_done[305][5] == 1) && (can_ignore[which] < 5))
		asyn = false;
	if(not_asyn)
		asyn = false;

	if((load_when_play[which]) && (sound_handles[which] == NULL)) {
			asyn = false;
		sprintf((char *)snd_name,"#%d",which + 1);
		h = FindResource(hModule,snd_name,"#100");

		sound_handles[which] = LoadResource(hModule,h);
		snds[which] = (char *) LockResource(sound_handles[which]);

		}

	if(store_last_sound_played == 6)
		sndPlaySound(NULL,0);

	if(asyn) {
		if(can_ignore[which] >= 4)
			check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY | SND_NOSTOP);
			else check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY);

		while(!check_sound) {

			if(can_ignore[store_last_sound_played] == 4) {// then sound goes away
				return;
				}


			num_fails++;
			if(num_fails < 40)
				sound_pause(25);
				else {
					MessageBox(mainPtr,"Cannot play sounds - Sounds stuck error a. Game can still be played, but quietly. Check to make sure your sound drivers are up to date and not corrupted.",
					  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
					print_nums(111,which,num_fails);
					sounds_fucked = true;
					return;
					}
			sndPlaySound(NULL,0);

			if(can_ignore[which] >= 4)
				check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY | SND_NOSTOP);
				else check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY);
			}
	  a_sound_did_get_played = true;
	  }
		else {
		if(can_ignore[which] >= 4)
			check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY | SND_NOSTOP);
			else check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY);
		while(!check_sound) {
			if(can_ignore[store_last_sound_played] == 4) {// then sound goes away
				return;
				}


			num_fails++;
			if(num_fails < 40)
				sound_pause(25);
				else {
					MessageBox(mainPtr,"Cannot play sounds - Sounds stuck error b. Game can still be played, but quietly. Check to make sure your sound drivers are up to date and not corrupted.",
					 "Sound Error",MB_OK | MB_ICONEXCLAMATION);
					print_nums(222,which,num_fails);
					sounds_fucked = true;
					return;
					}
			sndPlaySound(NULL,0);

			if(can_ignore[which] >= 4)
				check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY | SND_NOSTOP);
				else check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY);
			}
		a_sound_did_get_played = true;
	  }

	store_last_sound_played = which;

	if((load_when_play[which]) && !asyn)
		sound_handles[which] = NULL;
		
	for(i = 0; i < NUM_SOUNDS; i++)
		if((load_when_play[which]) && (sound_handles[which] != NULL)
			&& (a_sound_did_get_played) && (i != which)) {
			sound_handles[i] = NULL;
		}
#endif
}

#ifdef WIN32
void kill_sound() {
	sndPlaySound(NULL,0);
}
#endif

void one_sound(short which) {
	if(which == last_played)
		return;
	play_sound(which);
	last_played = which;
}

void clear_sound_memory(){
	last_played = 100;
}

void flip_sound() {
	play_sounds = (play_sounds) ? false : true;
}


#ifdef WIN32
void sound_pause(long len) {
	long t1,t2;

	t1 = (long) GetCurrentTime();
	t2 = t1;
	while(t2 - t1 < len) {
		t2 = (long)GetCurrentTime();
		}
}

void load_sounds(HMODULE handle) {
	short i,t,err;
	HRSRC h;
	char snd_name[20];
	WAVEOUTCAPS wavecaps;
	
	hModule = handle;

	t = waveOutGetNumDevs();
	if(t == 0) {
		sounds_missing = true;
		return;
	}
	err = waveOutGetDevCaps(0,&wavecaps,sizeof(WAVEOUTCAPS));
	if(err != 0) {
		sounds_missing = true;
		switch(err) {
		case MMSYSERR_BADDEVICEID:
			MessageBox(mainPtr,"Cannot initialize sounds - No sound device detected. Game can still be played, but quietly.",
		  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		case MMSYSERR_NODRIVER:
			MessageBox(mainPtr,"Cannot initialize sounds - No driver installed. Game can still be played, but quietly.",
		  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		case MMSYSERR_NOMEM :
			MessageBox(mainPtr,"Cannot initialize sounds - can't find enough memory. Game can still be played, but quietly.",
		  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		case MMSYSERR_ALLOCATED:
			MessageBox(mainPtr,"Cannot initialize sounds - sound card already allocated. Game can still be played, but quietly.",
		  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		case MMSYSERR_ERROR:
			MessageBox(mainPtr,"Cannot initialize sounds - internal error. Game can still be played, but quietly.",
		  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		default:
			MessageBox(mainPtr,"Cannot initialize sounds - unidentified error. Game can still be played, but quietly.",
		  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
			return;
	
		}
	
	}

	for(i = 0; i < NUM_SOUNDS; i++) {
		sound_handles[i] = NULL;
		if(!load_when_play[i]) {
			sprintf((char *)snd_name,"#%d",i + 1);
			h = FindResource(handle,snd_name,"#100");

			sound_handles[i] = LoadResource(handle,h);
			snds[i] = (char*) LockResource(sound_handles[i]);
		}
	}
}
#endif
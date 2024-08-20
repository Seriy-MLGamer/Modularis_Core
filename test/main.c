/*
(C) 2024 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is part of Modularis Core.
Modularis Core is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Modularis Core is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Modularis Core. If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <Modularis_Core/Modularis.h>
#include <Modularis_Core/modules/players/Sequencer.h>
#include <Modularis_Core/modules/effects/note/Note_chorus.h>
#include <Modularis_Core/modules/synthesizers/Oscillator.h>
#include <Modularis_Core/modules/effects/note/Transpose.h>
#include <Modularis_Core/modules/synthesizers/Sampler.h>
#include <Modularis_Core/modules/effects/sound/Amplifier.h>
#include <Modularis_Core/modules/effects/sound/Delay.h>
#include <Modularis_Core/modules/effects/sound/Modulator.h>
#include <locale.h>
#include <string.h>
#include <stdio.h>
#if defined(__linux__)
	#include <text GNU.h>
#elif defined(_WIN32)
	#include <text Windows.h>
#endif
#include <Modularis_Core/user/modules/players/Sequencer/Note_key.h>
#include <Modularis_Core/user/modules/players/Sequencer/Interpolation.h>
#include <Modularis_Core/user/modules/players/Sequencer/Discrete_key.h>
#include <Modularis_Core/user/modules/players/Sequencer/Continuous_key.h>
#include <Modularis_Core/user/modules/players/Sequencer/Sequence.h>
#include <Modularis_Core/user/modules/players/Sequencer/Pattern.h>
#include <Modularis_Core/user/modules/players/Sequencer/Pattern_none.h>
#include <SDL2/SDL.h>
#if !defined(NOGUI)
	#include <SDL2/SDL_opengl.h>
	#include <SDL2/SDL_image.h>
	#include <SDL2/SDL_ttf.h>
#endif

#define TEMPO 180.f
#define SONG_LENGTH 1776

uint32_t sample_rate=44100;
uint32_t buffer_size=1024;
uint32_t visualizer_time=20;
#define COVER_SIZE 512
#define PANEL_SIZE (COVER_SIZE/8+16)
#define WINDOW_WIDTH COVER_SIZE
#define WINDOW_HEIGHT (COVER_SIZE+PANEL_SIZE)
#define VISUALIZER_LINE_WIDTH 8
#define CURSOR_LINE_WIDTH 16
#define CURSOR_WIDTH 8
#define BUTTON_MARGIN 8
#define BUTTON_SIZE (WINDOW_HEIGHT-(COVER_SIZE+CURSOR_LINE_WIDTH+2*BUTTON_MARGIN))

//Не пугайтесь за кривой код. Я хотел по-быстренькому протестировать новейший функционал своей библиотеки.

uint32_t visualizer_size;
uint32_t visualizer_buffer_size;
#if !defined(NOGUI)
	bool nogui=false;
#endif
char *output=NULL;

inline int32_t clamp(int32_t value, int32_t min, int32_t max)
{
	return (value<min)*min+(value>max)*max+(value>=min&&value<=max)*value;
}
#if !defined(NOGUI)
	void circle(GLint x, GLint y, GLint diameter)
	{
		GLint vertices[8][2];
		for (int a=0; a!=8; a++)
		{
			vertices[a][0]=x+(GLint)(diameter*cosf((float)M_PI*a/4))/2;
			vertices[a][1]=y+(GLint)(diameter*sinf((float)M_PI*a/4))/2;
		}
		glVertexPointer(2, GL_INT, 0, vertices);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
	}
	void line(GLint x1, GLint y1, GLint x2, GLint y2, GLint width, bool curved1, bool curved2)
	{
		GLint delta_x=x2-x1;
		GLint delta_y=y2-y1;
		float length=sqrtf(delta_x*delta_x+delta_y*delta_y);
		float add_x=width*delta_x/(2*length);
		float add_y=width*delta_y/(2*length);
		GLint vertices[]={x1-add_y, y1+add_x, x1+add_y, y1-add_x, x2-add_y, y2+add_x, x2+add_y, y2-add_x};
		glVertexPointer(2, GL_INT, 0, vertices);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		if (curved1) circle(x1, y1, width);
		if (curved2) circle(x2, y2, width);
	}
	void box(GLint x, GLint y, GLint width, GLint height)
	{
		GLint vertices[]={x, y+1, x+width, y+1, x, y+height, x+width, y+height, x+1, y, x+1, y+height, x+width, y, x+width, y+height};
		glVertexPointer(2, GL_INT, 0, vertices);
		glDrawArrays(GL_LINES, 0, 8);
	}
	void square(GLint x, GLint y, GLint width, GLint height)
	{
		GLint vertices[]={x, y, x, y+height, x+width, y, x+width, y+height};
		glVertexPointer(2, GL_INT, 0, vertices);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	GLuint new_texture(SDL_Surface *surface, GLenum format)
	{
		GLuint result;
		glGenTextures(1, &result);
		glBindTexture(GL_TEXTURE_2D, result);
		glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glBindTexture(GL_TEXTURE_2D, 0);
		return result;
	}
	SDL_Surface *render_text(const char *text, int size, Uint32 width, GLuint *texture)
	{
		TTF_Font *font=TTF_OpenFont("share/MLGamers_monospace.ttf", size);
		SDL_Surface *temp;
		if (width) temp=TTF_RenderUTF8_Solid_Wrapped(font, text, (SDL_Color){255, 255, 255, 255}, width);
		else temp=TTF_RenderUTF8_Solid(font, text, (SDL_Color){255, 255, 255, 255});
		TTF_CloseFont(font);
		SDL_PixelFormat *format=SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
		SDL_Surface *result=SDL_ConvertSurface(temp, format, 0);
		SDL_FreeSurface(temp);
		SDL_FreeFormat(format);
		*texture=new_texture(result, GL_RGBA);
		return result;
	}
#endif

typedef struct Future_sound Future_sound;
struct Future_sound
{
	#if !defined(NOGUI)
		int16_t *visualizer_buffer;
		uint32_t visualizer_position;
		bool loaded;
		SDL_mutex *mutex;
	#endif
	MDLRS_Modularis project;
	MDLRS_Sequencer supersaw_bass_part;
	MDLRS_Note_chorus supersaw_maker;
	MDLRS_Oscillator supersaw_bass;
	MDLRS_Sequencer jumpy_bass_part;
	MDLRS_Oscillator jumpy_bass;
	MDLRS_Sequencer piano_part;
	MDLRS_Transpose piano_base_note;
	MDLRS_Sampler piano;
	MDLRS_Sequencer chords_part;
	MDLRS_Transpose chords2_offset, chords3_offset;
	MDLRS_Oscillator chords1, chords2, chords3;
	MDLRS_Amplifier chords2_pan, chords3_pan;
	MDLRS_Sequencer main_lead_part_single, main_lead_part;
	MDLRS_Transpose main_lead_plus_octave;
	MDLRS_Note_chorus main_lead_maker_single, main_lead_maker, main_lead_plus_octave_maker;
	MDLRS_Oscillator main_lead;
	MDLRS_Sequencer another_lead_part;
	MDLRS_Oscillator another_lead;
	MDLRS_Amplifier another_lead_pan;
	MDLRS_Sequencer kick_part;
	MDLRS_Sampler kick;
	MDLRS_Sequencer tom1_part, tom2_part;
	MDLRS_Sampler tom1, tom2;
	MDLRS_Amplifier tom1_pan, tom2_pan;
	MDLRS_Sequencer snare_part;
	MDLRS_Sampler snare;
	MDLRS_Sequencer hihat_part;
	MDLRS_Sampler hihat;
	MDLRS_Amplifier hihat_pan;
	MDLRS_Sequencer ride_part;
	MDLRS_Sampler ride;
	MDLRS_Amplifier ride_pan;
	MDLRS_Sequencer crash_part;
	MDLRS_Sampler crash;
	MDLRS_Amplifier crash_pan;
	MDLRS_Sequencer left_clap_part, right_clap_part;
	MDLRS_Sampler left_clap, right_clap;
	MDLRS_Amplifier left_clap_pan, right_clap_pan;
	MDLRS_Amplifier echo_wet_left, echo_wet_right;
	MDLRS_Delay echo_left_delay, echo_right_delay;
	MDLRS_Amplifier left_echo1, right_echo1;
	MDLRS_Amplifier left_echo2, right_echo2;
	MDLRS_Delay left_echo3, right_echo3;
	MDLRS_Sequencer sidechain_part;
	MDLRS_Oscillator sidechain;
	MDLRS_Modulator left_compressor, right_compressor;
};

void Future_sound_loop(Future_sound *self);
void Future_sound_new(Future_sound *self)
{
	#if !defined(NOGUI)
		self->visualizer_buffer=malloc(sizeof(int16_t)*visualizer_buffer_size);
		memset(self->visualizer_buffer, 0, sizeof(int16_t)*visualizer_buffer_size);
		self->visualizer_position=visualizer_size;
		self->loaded=false;
		self->mutex=SDL_CreateMutex();
	#endif
	MDLRS_Modularis_new(&self->project, sample_rate, 2);
	MDLRS_Sequencer_new(&self->supersaw_bass_part, &self->project);
	MDLRS_Note_chorus_new(&self->supersaw_maker, &self->project, .5f, 16);
	MDLRS_Oscillator_new(&self->supersaw_bass, &self->project);
	MDLRS_Sequencer_new(&self->jumpy_bass_part, &self->project);
	MDLRS_Oscillator_new(&self->jumpy_bass, &self->project);
	MDLRS_Sequencer_new(&self->piano_part, &self->project);
	MDLRS_Transpose_new(&self->piano_base_note, &self->project, -15);
	MDLRS_Sampler_new(&self->piano, &self->project, "share/samples/piano.wav");
	MDLRS_Sequencer_new(&self->chords_part, &self->project);
	MDLRS_Transpose_new(&self->chords2_offset, &self->project, -.05f);
	MDLRS_Transpose_new(&self->chords3_offset, &self->project, .05f);
	MDLRS_Oscillator_new(&self->chords1, &self->project);
	MDLRS_Oscillator_new(&self->chords2, &self->project);
	MDLRS_Oscillator_new(&self->chords3, &self->project);
	MDLRS_Amplifier_new(&self->chords2_pan, &self->project, .5f);
	MDLRS_Amplifier_new(&self->chords3_pan, &self->project, .5f);
	MDLRS_Sequencer_new(&self->main_lead_part_single, &self->project);
	MDLRS_Sequencer_new(&self->main_lead_part, &self->project);
	MDLRS_Transpose_new(&self->main_lead_plus_octave, &self->project, 12.1f);
	MDLRS_Note_chorus_new(&self->main_lead_maker_single, &self->project, .75f, 16);
	MDLRS_Note_chorus_new(&self->main_lead_maker, &self->project, .75f, 16);
	MDLRS_Note_chorus_new(&self->main_lead_plus_octave_maker, &self->project, .75f, 16);
	MDLRS_Oscillator_new(&self->main_lead, &self->project);
	MDLRS_Sequencer_new(&self->another_lead_part, &self->project);
	MDLRS_Oscillator_new(&self->another_lead, &self->project);
	MDLRS_Amplifier_new(&self->another_lead_pan, &self->project, .75f);
	MDLRS_Sequencer_new(&self->kick_part, &self->project);
	MDLRS_Sampler_new(&self->kick, &self->project, "share/samples/kick.wav");
	MDLRS_Sequencer_new(&self->tom1_part, &self->project);
	MDLRS_Sequencer_new(&self->tom2_part, &self->project);
	MDLRS_Sampler_new(&self->tom1, &self->project, "share/samples/tom.wav");
	MDLRS_Sampler_new(&self->tom2, &self->project, "share/samples/tom.wav");
	MDLRS_Amplifier_new(&self->tom1_pan, &self->project, .75f);
	MDLRS_Amplifier_new(&self->tom2_pan, &self->project, .75f);
	MDLRS_Sequencer_new(&self->snare_part, &self->project);
	MDLRS_Sampler_new(&self->snare, &self->project, "share/samples/snare.wav");
	MDLRS_Sequencer_new(&self->hihat_part, &self->project);
	MDLRS_Sampler_new(&self->hihat, &self->project, "share/samples/hihat.wav");
	MDLRS_Amplifier_new(&self->hihat_pan, &self->project, .5f);
	MDLRS_Sequencer_new(&self->ride_part, &self->project);
	MDLRS_Sampler_new(&self->ride, &self->project, "share/samples/ride.wav");
	MDLRS_Amplifier_new(&self->ride_pan, &self->project, .25f);
	MDLRS_Sequencer_new(&self->crash_part, &self->project);
	MDLRS_Sampler_new(&self->crash, &self->project, "share/samples/crash.wav");
	MDLRS_Amplifier_new(&self->crash_pan, &self->project, .25f);
	MDLRS_Sequencer_new(&self->left_clap_part, &self->project);
	MDLRS_Sequencer_new(&self->right_clap_part, &self->project);
	MDLRS_Sampler_new(&self->left_clap, &self->project, "share/samples/clap.wav");
	MDLRS_Sampler_new(&self->right_clap, &self->project, "share/samples/clap.wav");
	MDLRS_Amplifier_new(&self->left_clap_pan, &self->project, .25f);
	MDLRS_Amplifier_new(&self->right_clap_pan, &self->project, .25f);
	MDLRS_Amplifier_new(&self->echo_wet_left, &self->project, .75f);
	MDLRS_Amplifier_new(&self->echo_wet_right, &self->project, .75f);
	MDLRS_Delay_new(&self->echo_left_delay, &self->project, .25f);
	MDLRS_Delay_new(&self->echo_right_delay, &self->project, .5f);
	MDLRS_Amplifier_new(&self->left_echo1, &self->project, 1);
	MDLRS_Amplifier_new(&self->right_echo1, &self->project, 1);
	MDLRS_Amplifier_new(&self->left_echo2, &self->project, .5f);
	MDLRS_Amplifier_new(&self->right_echo2, &self->project, .5f);
	MDLRS_Delay_new(&self->left_echo3, &self->project, .5f);
	MDLRS_Delay_new(&self->right_echo3, &self->project, .5f);
	MDLRS_Sequencer_new(&self->sidechain_part, &self->project);
	MDLRS_Oscillator_new(&self->sidechain, &self->project);
	MDLRS_Modulator_new(&self->left_compressor, &self->project);
	MDLRS_Modulator_new(&self->right_compressor, &self->project);
	self->supersaw_bass_part.BPM.value=TEMPO;
	self->jumpy_bass_part.BPM.value=TEMPO;
	self->piano_part.BPM.value=TEMPO;
	self->chords_part.BPM.value=TEMPO;
	self->main_lead_part_single.BPM.value=TEMPO;
	self->main_lead_part.BPM.value=TEMPO;
	self->another_lead_part.BPM.value=TEMPO;
	self->kick_part.BPM.value=TEMPO;
	self->tom1_part.BPM.value=TEMPO;
	self->tom2_part.BPM.value=TEMPO;
	self->snare_part.BPM.value=TEMPO;
	self->hihat_part.BPM.value=TEMPO;
	self->ride_part.BPM.value=TEMPO;
	self->crash_part.BPM.value=TEMPO;
	self->left_clap_part.BPM.value=TEMPO;
	self->right_clap_part.BPM.value=TEMPO;
	self->sidechain_part.BPM.value=TEMPO;
	self->supersaw_bass.volume.value=.07f;
	self->supersaw_bass.waveform.value=2;
	self->jumpy_bass.volume.value=.3f;
	self->jumpy_bass.waveform.value=3;
	self->jumpy_bass.envelope.decay.value=.25f;
	self->jumpy_bass.envelope.sustain.value=0;
	self->piano.volume.value=.4f;
	self->piano.envelope.decay.value=2;
	self->piano.envelope.sustain.value=0;
	self->piano.loop.value=1;
	self->piano.loop_start.value=10012;
	self->piano.loop_length.value=10096-10012;
	self->chords1.volume.value=.09f;
	self->chords2.volume.value=.09f;
	self->chords3.volume.value=.09f;
	self->chords1.waveform.value=2;
	self->chords2.waveform.value=2;
	self->chords3.waveform.value=2;
	self->main_lead.volume.value=.06f;
	self->main_lead.waveform.value=2;
	self->main_lead.envelope.decay.value=.25f;
	self->main_lead.envelope.sustain.value=0;
	self->another_lead.volume.value=.25f;
	self->another_lead.waveform.value=2;
	self->kick.volume.value=.6f;
	self->kick.envelope.decay.value=1;
	self->tom1.volume.value=1.2f;
	self->tom2.volume.value=1.2f;
	self->tom1.envelope.decay.value=1;
	self->tom2.envelope.decay.value=1;
	self->snare.volume.value=.5f;
	self->snare.envelope.decay.value=1;
	self->hihat.volume.value=1;
	self->hihat.envelope.decay.value=1;
	self->ride.volume.value=.9f;
	self->ride.envelope.decay.value=1;
	self->crash.volume.value=.5f;
	self->crash.envelope.decay.value=1;
	self->left_clap.volume.value=.375f;
	self->right_clap.volume.value=.375f;
	self->left_clap.envelope.decay.value=1;
	self->right_clap.envelope.decay.value=1;
	self->sidechain.waveform.value=3;
	#define sidechain_depth .25f
	((MDLRS_Any_port_f)self->supersaw_bass_part.outputs.ports[0]->f)->connect(self->supersaw_bass_part.outputs.ports[0], self->supersaw_maker.inputs.ports[0]);
	((MDLRS_Any_port_f)self->supersaw_maker.outputs.ports[0]->f)->connect(self->supersaw_maker.outputs.ports[0], self->supersaw_bass.inputs.ports[0]);
	((MDLRS_Any_port_f)self->supersaw_bass.outputs.ports[0]->f)->connect(self->supersaw_bass.outputs.ports[0], self->echo_wet_left.inputs.ports[0]);
	((MDLRS_Any_port_f)self->supersaw_bass.outputs.ports[0]->f)->connect(self->supersaw_bass.outputs.ports[0], self->echo_wet_right.inputs.ports[0]);
	((MDLRS_Any_port_f)self->supersaw_bass.outputs.ports[0]->f)->connect(self->supersaw_bass.outputs.ports[0], self->left_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->supersaw_bass.outputs.ports[0]->f)->connect(self->supersaw_bass.outputs.ports[0], self->right_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->jumpy_bass_part.outputs.ports[0]->f)->connect(self->jumpy_bass_part.outputs.ports[0], self->jumpy_bass.inputs.ports[0]);
	((MDLRS_Any_port_f)self->jumpy_bass.outputs.ports[0]->f)->connect(self->jumpy_bass.outputs.ports[0], self->left_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->jumpy_bass.outputs.ports[0]->f)->connect(self->jumpy_bass.outputs.ports[0], self->right_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->piano_part.outputs.ports[0]->f)->connect(self->piano_part.outputs.ports[0], self->piano_base_note.inputs.ports[0]);
	((MDLRS_Any_port_f)self->piano_base_note.outputs.ports[0]->f)->connect(self->piano_base_note.outputs.ports[0], self->piano.inputs.ports[0]);
	((MDLRS_Any_port_f)self->piano.outputs.ports[0]->f)->connect(self->piano.outputs.ports[0], self->echo_wet_left.inputs.ports[0]);
	((MDLRS_Any_port_f)self->piano.outputs.ports[0]->f)->connect(self->piano.outputs.ports[0], self->echo_wet_right.inputs.ports[0]);
	((MDLRS_Any_port_f)self->piano.outputs.ports[0]->f)->connect(self->piano.outputs.ports[0], self->left_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->piano.outputs.ports[0]->f)->connect(self->piano.outputs.ports[0], self->right_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords_part.outputs.ports[0]->f)->connect(self->chords_part.outputs.ports[0], self->chords1.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords_part.outputs.ports[0]->f)->connect(self->chords_part.outputs.ports[0], self->chords2_offset.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords_part.outputs.ports[0]->f)->connect(self->chords_part.outputs.ports[0], self->chords3_offset.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords2_offset.outputs.ports[0]->f)->connect(self->chords2_offset.outputs.ports[0], self->chords2.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords3_offset.outputs.ports[0]->f)->connect(self->chords3_offset.outputs.ports[0], self->chords3.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords1.outputs.ports[0]->f)->connect(self->chords1.outputs.ports[0], self->left_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords1.outputs.ports[0]->f)->connect(self->chords1.outputs.ports[0], self->right_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords2.outputs.ports[0]->f)->connect(self->chords2.outputs.ports[0], self->left_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords2.outputs.ports[0]->f)->connect(self->chords2.outputs.ports[0], self->chords2_pan.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords2_pan.outputs.ports[0]->f)->connect(self->chords2_pan.outputs.ports[0], self->right_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords3.outputs.ports[0]->f)->connect(self->chords3.outputs.ports[0], self->chords3_pan.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords3.outputs.ports[0]->f)->connect(self->chords3.outputs.ports[0], self->right_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->chords3_pan.outputs.ports[0]->f)->connect(self->chords3_pan.outputs.ports[0], self->left_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead_part_single.outputs.ports[0]->f)->connect(self->main_lead_part_single.outputs.ports[0], self->main_lead_maker_single.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead_maker_single.outputs.ports[0]->f)->connect(self->main_lead_maker_single.outputs.ports[0], self->main_lead.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead_part.outputs.ports[0]->f)->connect(self->main_lead_part.outputs.ports[0], self->main_lead_maker.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead_part.outputs.ports[0]->f)->connect(self->main_lead_part.outputs.ports[0], self->main_lead_plus_octave.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead_plus_octave.outputs.ports[0]->f)->connect(self->main_lead_plus_octave.outputs.ports[0], self->main_lead_plus_octave_maker.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead_maker.outputs.ports[0]->f)->connect(self->main_lead_maker.outputs.ports[0], self->main_lead.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead_plus_octave_maker.outputs.ports[0]->f)->connect(self->main_lead_plus_octave_maker.outputs.ports[0], self->main_lead.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead.outputs.ports[0]->f)->connect(self->main_lead.outputs.ports[0], self->echo_wet_left.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead.outputs.ports[0]->f)->connect(self->main_lead.outputs.ports[0], self->echo_wet_right.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead.outputs.ports[0]->f)->connect(self->main_lead.outputs.ports[0], self->left_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->main_lead.outputs.ports[0]->f)->connect(self->main_lead.outputs.ports[0], self->right_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->another_lead_part.outputs.ports[0]->f)->connect(self->another_lead_part.outputs.ports[0], self->another_lead.inputs.ports[0]);
	((MDLRS_Any_port_f)self->another_lead.outputs.ports[0]->f)->connect(self->another_lead.outputs.ports[0], self->another_lead_pan.inputs.ports[0]);
	((MDLRS_Any_port_f)self->another_lead.outputs.ports[0]->f)->connect(self->another_lead.outputs.ports[0], self->echo_wet_right.inputs.ports[0]);
	((MDLRS_Any_port_f)self->another_lead.outputs.ports[0]->f)->connect(self->another_lead.outputs.ports[0], self->right_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->another_lead_pan.outputs.ports[0]->f)->connect(self->another_lead_pan.outputs.ports[0], self->left_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->another_lead_pan.outputs.ports[0]->f)->connect(self->another_lead_pan.outputs.ports[0], self->echo_wet_left.inputs.ports[0]);
	((MDLRS_Any_port_f)self->kick_part.outputs.ports[0]->f)->connect(self->kick_part.outputs.ports[0], self->kick.inputs.ports[0]);
	((MDLRS_Any_port_f)self->kick.outputs.ports[0]->f)->connect(self->kick.outputs.ports[0], (MDLRS_Any_port *)&self->project.output.inputs);
	((MDLRS_Any_port_f)self->tom1_part.outputs.ports[0]->f)->connect(self->tom1_part.outputs.ports[0], self->tom1.inputs.ports[0]);
	((MDLRS_Any_port_f)self->tom2_part.outputs.ports[0]->f)->connect(self->tom2_part.outputs.ports[0], self->tom2.inputs.ports[0]);
	((MDLRS_Any_port_f)self->tom1.outputs.ports[0]->f)->connect(self->tom1.outputs.ports[0], self->tom1_pan.inputs.ports[0]);
	((MDLRS_Any_port_f)self->tom1.outputs.ports[0]->f)->connect(self->tom1.outputs.ports[0], self->project.output.inputs.ports[1]);
	((MDLRS_Any_port_f)self->tom1_pan.outputs.ports[0]->f)->connect(self->tom1_pan.outputs.ports[0], self->project.output.inputs.ports[0]);
	((MDLRS_Any_port_f)self->tom2.outputs.ports[0]->f)->connect(self->tom2.outputs.ports[0], self->project.output.inputs.ports[0]);
	((MDLRS_Any_port_f)self->tom2.outputs.ports[0]->f)->connect(self->tom2.outputs.ports[0], self->tom2_pan.inputs.ports[0]);
	((MDLRS_Any_port_f)self->tom2_pan.outputs.ports[0]->f)->connect(self->tom2_pan.outputs.ports[0], self->project.output.inputs.ports[1]);
	((MDLRS_Any_port_f)self->snare_part.outputs.ports[0]->f)->connect(self->snare_part.outputs.ports[0], self->snare.inputs.ports[0]);
	((MDLRS_Any_port_f)self->snare.outputs.ports[0]->f)->connect(self->snare.outputs.ports[0], (MDLRS_Any_port *)&self->project.output.inputs);
	((MDLRS_Any_port_f)self->hihat_part.outputs.ports[0]->f)->connect(self->hihat_part.outputs.ports[0], self->hihat.inputs.ports[0]);
	((MDLRS_Any_port_f)self->hihat.outputs.ports[0]->f)->connect(self->hihat.outputs.ports[0], self->hihat_pan.inputs.ports[0]);
	((MDLRS_Any_port_f)self->hihat.outputs.ports[0]->f)->connect(self->hihat.outputs.ports[0], self->project.output.inputs.ports[1]);
	((MDLRS_Any_port_f)self->hihat_pan.outputs.ports[0]->f)->connect(self->hihat_pan.outputs.ports[0], self->project.output.inputs.ports[0]);
	((MDLRS_Any_port_f)self->ride_part.outputs.ports[0]->f)->connect(self->ride_part.outputs.ports[0], self->ride.inputs.ports[0]);
	((MDLRS_Any_port_f)self->ride.outputs.ports[0]->f)->connect(self->ride.outputs.ports[0], self->project.output.inputs.ports[0]);
	((MDLRS_Any_port_f)self->ride.outputs.ports[0]->f)->connect(self->ride.outputs.ports[0], self->ride_pan.inputs.ports[0]);
	((MDLRS_Any_port_f)self->ride_pan.outputs.ports[0]->f)->connect(self->ride_pan.outputs.ports[0], self->project.output.inputs.ports[1]);
	((MDLRS_Any_port_f)self->crash_part.outputs.ports[0]->f)->connect(self->crash_part.outputs.ports[0], self->crash.inputs.ports[0]);
	((MDLRS_Any_port_f)self->crash.outputs.ports[0]->f)->connect(self->crash.outputs.ports[0], self->crash_pan.inputs.ports[0]);
	((MDLRS_Any_port_f)self->crash.outputs.ports[0]->f)->connect(self->crash.outputs.ports[0], self->project.output.inputs.ports[1]);
	((MDLRS_Any_port_f)self->crash_pan.outputs.ports[0]->f)->connect(self->crash_pan.outputs.ports[0], self->project.output.inputs.ports[0]);
	((MDLRS_Any_port_f)self->left_clap_part.outputs.ports[0]->f)->connect(self->left_clap_part.outputs.ports[0], self->left_clap.inputs.ports[0]);
	((MDLRS_Any_port_f)self->right_clap_part.outputs.ports[0]->f)->connect(self->right_clap_part.outputs.ports[0], self->right_clap.inputs.ports[0]);
	((MDLRS_Any_port_f)self->left_clap.outputs.ports[0]->f)->connect(self->left_clap.outputs.ports[0], self->project.output.inputs.ports[0]);
	((MDLRS_Any_port_f)self->left_clap.outputs.ports[0]->f)->connect(self->left_clap.outputs.ports[0], self->left_clap_pan.inputs.ports[0]);
	((MDLRS_Any_port_f)self->left_clap_pan.outputs.ports[0]->f)->connect(self->left_clap_pan.outputs.ports[0], self->project.output.inputs.ports[1]);
	((MDLRS_Any_port_f)self->right_clap.outputs.ports[0]->f)->connect(self->right_clap.outputs.ports[0], self->right_clap_pan.inputs.ports[0]);
	((MDLRS_Any_port_f)self->right_clap.outputs.ports[0]->f)->connect(self->right_clap.outputs.ports[0], self->project.output.inputs.ports[1]);
	((MDLRS_Any_port_f)self->right_clap_pan.outputs.ports[0]->f)->connect(self->right_clap_pan.outputs.ports[0], self->project.output.inputs.ports[0]);
	((MDLRS_Any_port_f)self->echo_wet_left.outputs.ports[0]->f)->connect(self->echo_wet_left.outputs.ports[0], self->echo_left_delay.inputs.ports[0]);
	((MDLRS_Any_port_f)self->echo_wet_right.outputs.ports[0]->f)->connect(self->echo_wet_right.outputs.ports[0], self->echo_right_delay.inputs.ports[0]);
	((MDLRS_Any_port_f)self->echo_left_delay.outputs.ports[0]->f)->connect(self->echo_left_delay.outputs.ports[0], self->left_echo1.inputs.ports[0]);
	((MDLRS_Any_port_f)self->echo_right_delay.outputs.ports[0]->f)->connect(self->echo_right_delay.outputs.ports[0], self->right_echo1.inputs.ports[0]);
	((MDLRS_Any_port_f)self->left_echo1.outputs.ports[0]->f)->connect(self->left_echo1.outputs.ports[0], self->left_echo2.inputs.ports[0]);
	((MDLRS_Any_port_f)self->right_echo1.outputs.ports[0]->f)->connect(self->right_echo1.outputs.ports[0], self->right_echo2.inputs.ports[0]);
	((MDLRS_Any_port_f)self->left_echo2.outputs.ports[0]->f)->connect(self->left_echo2.outputs.ports[0], self->left_echo3.inputs.ports[0]);
	((MDLRS_Any_port_f)self->right_echo2.outputs.ports[0]->f)->connect(self->right_echo2.outputs.ports[0], self->right_echo3.inputs.ports[0]);
	((MDLRS_Any_port_f)self->left_echo3.outputs.ports[0]->f)->connect(self->left_echo3.outputs.ports[0], self->left_echo1.inputs.ports[0]);
	((MDLRS_Any_port_f)self->right_echo3.outputs.ports[0]->f)->connect(self->right_echo3.outputs.ports[0], self->right_echo1.inputs.ports[0]);
	((MDLRS_Any_port_f)self->left_echo1.outputs.ports[0]->f)->connect(self->left_echo1.outputs.ports[0], self->left_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->right_echo1.outputs.ports[0]->f)->connect(self->right_echo1.outputs.ports[0], self->right_compressor.inputs.ports[0]);
	((MDLRS_Any_port_f)self->sidechain_part.outputs.ports[0]->f)->connect(self->sidechain_part.outputs.ports[0], self->sidechain.inputs.ports[0]);
	((MDLRS_Any_port_f)self->left_compressor.outputs.ports[0]->f)->connect(self->left_compressor.outputs.ports[0], self->project.output.inputs.ports[0]);
	((MDLRS_Any_port_f)self->right_compressor.outputs.ports[0]->f)->connect(self->right_compressor.outputs.ports[0], self->project.output.inputs.ports[1]);
	Future_sound_loop(self);
}
void Future_sound_play(Future_sound *self)
{
	#if !defined(NOGUI)
		SDL_LockMutex(self->mutex);
	#endif
	self->supersaw_bass_part.play.value=1;
	self->jumpy_bass_part.play.value=1;
	self->piano_part.play.value=1;
	self->chords_part.play.value=1;
	self->main_lead_part_single.play.value=1;
	self->main_lead_part.play.value=1;
	self->another_lead_part.play.value=1;
	self->kick_part.play.value=1;
	self->tom1_part.play.value=1;
	self->tom2_part.play.value=1;
	self->snare_part.play.value=1;
	self->hihat_part.play.value=1;
	self->ride_part.play.value=1;
	self->crash_part.play.value=1;
	self->left_clap_part.play.value=1;
	self->right_clap_part.play.value=1;
	self->sidechain_part.play.value=1;
	((MDLRS_Any_port_f)self->sidechain.outputs.ports[0]->f)->connect(self->sidechain.outputs.ports[0], self->left_compressor.inputs.ports[1]);
	((MDLRS_Any_port_f)self->sidechain.outputs.ports[0]->f)->connect(self->sidechain.outputs.ports[0], self->right_compressor.inputs.ports[1]);
	#if !defined(NOGUI)
		SDL_UnlockMutex(self->mutex);
	#endif
}
#if !defined(NOGUI)
	void Future_sound_pause(Future_sound *self)
	{
		SDL_LockMutex(self->mutex);
		self->supersaw_bass_part.play.value=0;
		self->jumpy_bass_part.play.value=0;
		self->piano_part.play.value=0;
		self->chords_part.play.value=0;
		self->main_lead_part_single.play.value=0;
		self->main_lead_part.play.value=0;
		self->another_lead_part.play.value=0;
		self->kick_part.play.value=0;
		self->tom1_part.play.value=0;
		self->tom2_part.play.value=0;
		self->snare_part.play.value=0;
		self->hihat_part.play.value=0;
		self->ride_part.play.value=0;
		self->crash_part.play.value=0;
		self->left_clap_part.play.value=0;
		self->right_clap_part.play.value=0;
		self->sidechain_part.play.value=0;
		((MDLRS_Any_port_f)self->sidechain.outputs.ports[0]->f)->disconnect(self->sidechain.outputs.ports[0]);
		SDL_UnlockMutex(self->mutex);
	}
#endif
void Future_sound_loop(Future_sound *self)
{
	#if !defined(NOGUI)
		SDL_LockMutex(self->mutex);
	#endif
	self->supersaw_bass_part.loop.value=1;
	self->jumpy_bass_part.loop.value=1;
	self->piano_part.loop.value=1;
	self->chords_part.loop.value=1;
	self->main_lead_part_single.loop.value=1;
	self->main_lead_part.loop.value=1;
	self->another_lead_part.loop.value=1;
	self->kick_part.loop.value=1;
	self->tom1_part.loop.value=1;
	self->tom2_part.loop.value=1;
	self->snare_part.loop.value=1;
	self->hihat_part.loop.value=1;
	self->ride_part.loop.value=1;
	self->crash_part.loop.value=1;
	self->left_clap_part.loop.value=1;
	self->right_clap_part.loop.value=1;
	self->sidechain_part.loop.value=1;
	#if !defined(NOGUI)
		SDL_UnlockMutex(self->mutex);
	#endif
}
#if !defined(NOGUI)
	void Future_sound_unloop(Future_sound *self)
	{
		SDL_LockMutex(self->mutex);
		self->supersaw_bass_part.loop.value=0;
		self->jumpy_bass_part.loop.value=0;
		self->piano_part.loop.value=0;
		self->chords_part.loop.value=0;
		self->main_lead_part_single.loop.value=0;
		self->main_lead_part.loop.value=0;
		self->another_lead_part.loop.value=0;
		self->kick_part.loop.value=0;
		self->tom1_part.loop.value=0;
		self->tom2_part.loop.value=0;
		self->snare_part.loop.value=0;
		self->hihat_part.loop.value=0;
		self->ride_part.loop.value=0;
		self->crash_part.loop.value=0;
		self->left_clap_part.loop.value=0;
		self->right_clap_part.loop.value=0;
		self->sidechain_part.loop.value=0;
		SDL_UnlockMutex(self->mutex);
	}
	void Future_sound_set_position(Future_sound *self, float position)
	{
		if (position<0) position=0;
		SDL_LockMutex(self->mutex);
		MDLRS_Sequencer_set_position(&self->supersaw_bass_part, position);
		MDLRS_Sequencer_set_position(&self->jumpy_bass_part, position);
		MDLRS_Sequencer_set_position(&self->piano_part, position);
		MDLRS_Sequencer_set_position(&self->chords_part, position);
		MDLRS_Sequencer_set_position(&self->main_lead_part_single, position);
		MDLRS_Sequencer_set_position(&self->main_lead_part, position);
		MDLRS_Sequencer_set_position(&self->another_lead_part, position);
		MDLRS_Sequencer_set_position(&self->kick_part, position);
		MDLRS_Sequencer_set_position(&self->tom1_part, position);
		MDLRS_Sequencer_set_position(&self->tom2_part, position);
		MDLRS_Sequencer_set_position(&self->snare_part, position);
		MDLRS_Sequencer_set_position(&self->hihat_part, position);
		MDLRS_Sequencer_set_position(&self->ride_part, position);
		MDLRS_Sequencer_set_position(&self->crash_part, position);
		MDLRS_Sequencer_set_position(&self->left_clap_part, position);
		MDLRS_Sequencer_set_position(&self->right_clap_part, position);
		MDLRS_Sequencer_set_position(&self->sidechain_part, position);
		SDL_UnlockMutex(self->mutex);
	}
#endif
void Future_sound_remove(Future_sound *self)
{
	#if !defined(NOGUI)
		free(self->visualizer_buffer);
		SDL_DestroyMutex(self->mutex);
	#endif
	MDLRS_Sequencer_remove(&self->supersaw_bass_part);
	MDLRS_Note_chorus_remove(&self->supersaw_maker);
	MDLRS_Oscillator_remove(&self->supersaw_bass);
	MDLRS_Sequencer_remove(&self->jumpy_bass_part);
	MDLRS_Oscillator_remove(&self->jumpy_bass);
	MDLRS_Sequencer_remove(&self->piano_part);
	MDLRS_Transpose_remove(&self->piano_base_note);
	MDLRS_Sampler_remove(&self->piano);
	MDLRS_Sequencer_remove(&self->chords_part);
	MDLRS_Transpose_remove(&self->chords2_offset);
	MDLRS_Transpose_remove(&self->chords3_offset);
	MDLRS_Oscillator_remove(&self->chords1);
	MDLRS_Oscillator_remove(&self->chords2);
	MDLRS_Oscillator_remove(&self->chords3);
	MDLRS_Amplifier_remove(&self->chords2_pan);
	MDLRS_Amplifier_remove(&self->chords3_pan);
	MDLRS_Sequencer_remove(&self->main_lead_part_single);
	MDLRS_Sequencer_remove(&self->main_lead_part);
	MDLRS_Transpose_remove(&self->main_lead_plus_octave);
	MDLRS_Note_chorus_remove(&self->main_lead_maker_single);
	MDLRS_Note_chorus_remove(&self->main_lead_maker);
	MDLRS_Note_chorus_remove(&self->main_lead_plus_octave_maker);
	MDLRS_Oscillator_remove(&self->main_lead);
	MDLRS_Sequencer_remove(&self->another_lead_part);
	MDLRS_Oscillator_remove(&self->another_lead);
	MDLRS_Amplifier_remove(&self->another_lead_pan);
	MDLRS_Sequencer_remove(&self->kick_part);
	MDLRS_Sampler_remove(&self->kick);
	MDLRS_Sequencer_remove(&self->tom1_part);
	MDLRS_Sequencer_remove(&self->tom2_part);
	MDLRS_Sampler_remove(&self->tom1);
	MDLRS_Sampler_remove(&self->tom2);
	MDLRS_Amplifier_remove(&self->tom1_pan);
	MDLRS_Amplifier_remove(&self->tom2_pan);
	MDLRS_Sequencer_remove(&self->snare_part);
	MDLRS_Sampler_remove(&self->snare);
	MDLRS_Sequencer_remove(&self->hihat_part);
	MDLRS_Sampler_remove(&self->hihat);
	MDLRS_Amplifier_remove(&self->hihat_pan);
	MDLRS_Sequencer_remove(&self->ride_part);
	MDLRS_Sampler_remove(&self->ride);
	MDLRS_Amplifier_remove(&self->ride_pan);
	MDLRS_Sequencer_remove(&self->crash_part);
	MDLRS_Sampler_remove(&self->crash);
	MDLRS_Amplifier_remove(&self->crash_pan);
	MDLRS_Sequencer_remove(&self->left_clap_part);
	MDLRS_Sequencer_remove(&self->right_clap_part);
	MDLRS_Sampler_remove(&self->left_clap);
	MDLRS_Sampler_remove(&self->right_clap);
	MDLRS_Amplifier_remove(&self->left_clap_pan);
	MDLRS_Amplifier_remove(&self->right_clap_pan);
	MDLRS_Amplifier_remove(&self->echo_wet_left);
	MDLRS_Amplifier_remove(&self->echo_wet_right);
	MDLRS_Delay_remove(&self->echo_left_delay);
	MDLRS_Delay_remove(&self->echo_right_delay);
	MDLRS_Amplifier_remove(&self->left_echo1);
	MDLRS_Amplifier_remove(&self->right_echo1);
	MDLRS_Amplifier_remove(&self->left_echo2);
	MDLRS_Amplifier_remove(&self->right_echo2);
	MDLRS_Delay_remove(&self->left_echo3);
	MDLRS_Delay_remove(&self->right_echo3);
	MDLRS_Sequencer_remove(&self->sidechain_part);
	MDLRS_Oscillator_remove(&self->sidechain);
	MDLRS_Modulator_remove(&self->left_compressor);
	MDLRS_Modulator_remove(&self->right_compressor);
	MDLRS_Modularis_remove(&self->project);
}
#if !defined(NOGUI)
	typedef struct Button Button;
	struct Button
	{
		void *f;

		GLint x, y, width, height;
		bool hover;
		bool pressed;
		bool mouse;
		bool key;
	};
	typedef struct Button_f *Button_f;
	struct Button_f
	{
		void (*on_click)(void *self, Future_sound *song);
	};
	void Button_new(Button *self, GLint x, GLint y, GLint width, GLint height)
	{
		self->x=x;
		self->y=y;
		self->width=width;
		self->height=height;
		self->hover=false;
		self->pressed=false;
		self->mouse=false;
		self->key=false;
	}
	void Button_draw(Button *self)
	{
		if (self->hover||self->key)
		{
			if (self->pressed) glColor3ub(0, 32, 0);
			else glColor3ub(32, 64, 32);
		}
		else glColor3ub(0, 64, 0);
		square(self->x, self->y, self->width, self->height);
		if (self->hover||self->key)
		{
			if (self->pressed) glColor3ub(0, 128, 0);
			else glColor3ub(128, 255, 128);
		}
		else glColor3ub(0, 255, 0);
		box(self->x, self->y, self->width, self->height);
	}
	void Button_hover(Button *self, SDL_MouseMotionEvent *event)
	{
		self->hover=event->x>=self->x&&event->x<self->x+self->width&&event->y>=self->y&&event->y<self->y+self->height;
	}
	void Button_press(Button *self, SDL_MouseButtonEvent *event)
	{
		if (event->button==1)
		{
			self->pressed=self->key||self->hover;
			self->mouse=self->hover;
		}
	}
	void Button_release(Button *self, SDL_MouseButtonEvent *event, Future_sound *song)
	{
		if (self->pressed) if (event->button==1)
		{
			if (self->hover) ((Button_f)self->f)->on_click(self, song);
			self->pressed=self->key;
			self->mouse=false;
		}
	}
	void Button_key_press(Button *self, Future_sound *song)
	{
		((Button_f)self->f)->on_click(self, song);
		self->pressed=true;
		self->key=true;
	}
	void Button_key_release(Button *self)
	{
		self->pressed=self->mouse;
		self->key=false;
	}
	typedef struct Play Play;
	struct Play
	{
		Button;

		bool play;
	};
	void Play_on_click(Play *self, Future_sound *song);
	struct Play Play_f=
	{
		(void (*)(void *, Future_sound *))Play_on_click
	};
	void Play_new(Play *self, GLint x, GLint y, GLint width, GLint height)
	{
		Button_new((Button *)self, x, y, width, height);
		self->f=&Play_f;

		self->play=true;
	}
	void Play_draw(Play *self)
	{
		if (self->play)
		{
			if (self->hover||self->key)
			{
				if (self->pressed) glColor3ub(32, 32, 0);
				else glColor3ub(64, 64, 32);
			}
			else glColor3ub(64, 64, 0);
			square(self->x, self->y, self->width, self->height);
			if (self->hover||self->key)
			{
				if (self->pressed) glColor3ub(128, 128, 0);
				else glColor3ub(255, 255, 128);
			}
			else glColor3ub(255, 255, 0);
			box(self->x, self->y, self->width, self->height);
			glColor3ub(255, 255, 255);
			glBegin(GL_TRIANGLE_STRIP);
				glVertex2i(self->x+self->width/4, self->y+self->height/4);
				glVertex2i(self->x+self->width/4, self->y+3*self->height/4);
				glVertex2i(self->x+5*self->width/12, self->y+self->height/4);
				glVertex2i(self->x+5*self->width/12, self->y+3*self->height/4);
			glEnd();
			glBegin(GL_TRIANGLE_STRIP);
				glVertex2i(self->x+7*self->width/12, self->y+self->height/4);
				glVertex2i(self->x+7*self->width/12, self->y+3*self->height/4);
				glVertex2i(self->x+3*self->width/4, self->y+self->height/4);
				glVertex2i(self->x+3*self->width/4, self->y+3*self->height/4);
			glEnd();
		}
		else
		{
			Button_draw((Button *)self);
			glColor3ub(255, 255, 255);
			glBegin(GL_TRIANGLES);
				glVertex2i(self->x+self->width/4, self->y+(self->height+self->width/sqrtf(3))/2);
				glVertex2i(self->x+self->width/4, self->y+(self->height-self->width/sqrtf(3))/2);
				glVertex2i(self->x+3*self->width/4, self->y+self->height/2);
			glEnd();
		}
	}
	void Play_on_click(Play *self, Future_sound *song)
	{
		if (self->play)
		{
			Future_sound_pause(song);
			self->play=false;
		}
		else
		{
			Future_sound_play(song);
			self->play=true;
		}
	}
	typedef struct Backward Backward;
	struct Backward
	{
		Button;
	};
	void Backward_on_click(Backward *self, Future_sound *song);
	struct Backward Backward_f=
	{
		(void (*)(void *, Future_sound *))Backward_on_click
	};
	void Backward_new(Backward *self, GLint x, GLint y, GLint width, GLint height)
	{
		Button_new((Button *)self, x, y, width, height);
		self->f=&Backward_f;
	}
	void Backward_draw(Backward *self)
	{
		Button_draw((Button *)self);
		glColor3ub(255, 255, 255);
		glBegin(GL_TRIANGLES);
			glVertex2i(self->x+self->width/2, self->y+self->height/2);
			glVertex2i(self->x+3*self->width/4, self->y+(2*self->height-self->width/sqrtf(3))/4);
			glVertex2i(self->x+3*self->width/4, self->y+(2*self->height+self->width/sqrtf(3))/4);
			glVertex2i(self->x+self->width/4, self->y+self->height/2);
			glVertex2i(self->x+self->width/2, self->y+(2*self->height-self->width/sqrtf(3))/4);
			glVertex2i(self->x+self->width/2, self->y+(2*self->height+self->width/sqrtf(3))/4);
		glEnd();
	}
	void Backward_on_click(Backward *self, Future_sound *song)
	{
		Future_sound_set_position(song, song->sidechain_part.cursor_position.value-10*8*TEMPO/60);
	}
	typedef struct Forward Forward;
	struct Forward
	{
		Button;
	};
	void Forward_on_click(Forward *self, Future_sound *song);
	struct Forward Forward_f=
	{
		(void (*)(void *, Future_sound *))Forward_on_click
	};
	void Forward_new(Forward *self, GLint x, GLint y, GLint width, GLint height)
	{
		Button_new((Button *)self, x, y, width, height);
		self->f=&Forward_f;
	}
	void Forward_draw(Forward *self)
	{
		Button_draw((Button *)self);
		glColor3ub(255, 255, 255);
		glBegin(GL_TRIANGLES);
			glVertex2i(self->x+self->width/4, self->y+(2*self->height+self->width/sqrtf(3))/4);
			glVertex2i(self->x+self->width/4, self->y+(2*self->height-self->width/sqrtf(3))/4);
			glVertex2i(self->x+self->width/2, self->y+self->height/2);
			glVertex2i(self->x+self->width/2, self->y+(2*self->height+self->width/sqrtf(3))/4);
			glVertex2i(self->x+self->width/2, self->y+(2*self->height-self->width/sqrtf(3))/4);
			glVertex2i(self->x+3*self->width/4, self->y+self->height/2);
		glEnd();
	}
	void Forward_on_click(Forward *self, Future_sound *song)
	{
		Future_sound_set_position(song, song->sidechain_part.cursor_position.value+10*8*TEMPO/60);
	}
	typedef struct Loop Loop;
	struct Loop
	{
		Button;

		bool loop;
	};
	void Loop_on_click(Loop *self, Future_sound *song);
	struct Loop Loop_f=
	{
		(void (*)(void *, Future_sound *))Loop_on_click
	};
	void Loop_new(Loop *self, GLint x, GLint y, GLint width, GLint height)
	{
		Button_new((Button *)self, x, y, width, height);
		self->f=&Loop_f;

		self->loop=true;
	}
	void Loop_draw(Loop *self)
	{
		if (self->loop)
		{
			if (self->hover||self->key)
			{
				if (self->pressed) glColor3ub(32, 32, 0);
				else glColor3ub(64, 64, 32);
			}
			else glColor3ub(64, 64, 0);
			square(self->x, self->y, self->width, self->height);
			if (self->hover||self->key)
			{
				if (self->pressed) glColor3ub(128, 128, 0);
				else glColor3ub(255, 255, 128);
			}
			else glColor3ub(255, 255, 0);
			box(self->x, self->y, self->width, self->height);
		}
		else Button_draw((Button *)self);
		glColor3ub(255, 255, 255);
		for (int a=0; a!=6; a++) line(self->x+self->width/2-3*self->width*sinf(M_PI*a/4)/16, self->y+self->width/2+3*self->width*cosf(M_PI*a/4)/16, self->x+self->width/2-3*self->width*sinf(M_PI*(a+1)/4)/16, self->y+self->width/2+3*self->width*cosf(M_PI*(a+1)/4)/16, self->width/8, false, true);
		glBegin(GL_TRIANGLES);
			glVertex2i(self->x+13*self->width/16, self->y+self->height/2);
			glVertex2i(self->x+9*self->width/16, self->y+self->height/2);
			glVertex2i(self->x+11*self->width/16, self->y+(4+sqrtf(3))*self->height/8);
		glEnd();
		if (self->loop) line(self->x+3*self->width/4, self->y+self->height/4, self->x+self->width/4, self->y+3*self->height/4, self->width/8, false, false);
	}
	void Loop_on_click(Loop *self, Future_sound *song)
	{
		if (self->loop)
		{
			Future_sound_unloop(song);
			self->loop=false;
		}
		else
		{
			Future_sound_loop(song);
			self->loop=true;
		}
	}
	typedef struct Info Info;
	struct Info
	{
		Button;

		SDL_Surface *text_shown, *text_hidden;
		GLuint texture_shown, texture_hidden;
		bool show;
	};
	void Info_on_click(Info *self, Future_sound *song);
	struct Info Info_f=
	{
		(void (*)(void *, Future_sound *))Info_on_click
	};
	void Info_new(Info *self, GLint x, GLint y, GLint width, GLint height)
	{
		Button_new((Button *)self, x, y, width, height);
		self->f=&Info_f;

		self->text_shown=render_text("Скрыть", 16, 0, &self->texture_shown);
		self->text_hidden=render_text("О программе", 16, 0, &self->texture_hidden);
		self->show=false;
	}
	void Info_draw(Info *self)
	{
		if (self->show)
		{
			if (self->hover||self->key)
			{
				if (self->pressed) glColor3ub(32, 32, 0);
				else glColor3ub(64, 64, 32);
			}
			else glColor3ub(64, 64, 0);
			square(self->x, self->y, self->width, self->height);
			if (self->hover||self->key)
			{
				if (self->pressed) glColor3ub(128, 128, 0);
				else glColor3ub(255, 255, 128);
			}
			else glColor3ub(255, 255, 0);
			box(self->x, self->y, self->width, self->height);
		}
		else Button_draw((Button *)self);
		glColor3ub(255, 255, 255);
		SDL_Surface *text;
		if (self->show)
		{
			text=self->text_shown;
			glBindTexture(GL_TEXTURE_2D, self->texture_shown);
		}
		else
		{
			text=self->text_hidden;
			glBindTexture(GL_TEXTURE_2D, self->texture_hidden);
		}
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2i(0, 0); glVertex2i(self->x+(self->width-text->w)/2, self->y+(self->height-text->h)/2);
			glTexCoord2i(0, 1); glVertex2i(self->x+(self->width-text->w)/2, self->y+(self->height+text->h)/2);
			glTexCoord2i(1, 0); glVertex2i(self->x+(self->width+text->w)/2, self->y+(self->height-text->h)/2);
			glTexCoord2i(1, 1); glVertex2i(self->x+(self->width+text->w)/2, self->y+(self->height+text->h)/2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void Info_on_click(Info *self, Future_sound *song)
	{
		self->show=!self->show;
	}
	void Info_remove(Info *self)
	{
		glDeleteTextures(1, &self->texture_shown);
		glDeleteTextures(1, &self->texture_hidden);
		SDL_FreeSurface(self->text_shown);
		SDL_FreeSurface(self->text_hidden);
	}
	typedef struct Render Render;
	struct Render
	{
		Button;

		SDL_Surface *text;
		GLuint texture;
		bool render;
	};
	void Render_on_click(Render *self, Future_sound *song);
	struct Render Render_f=
	{
		(void (*)(void *, Future_sound *))Render_on_click
	};
	void Render_new(Render *self, GLint x, GLint y, GLint width, GLint height)
	{
		Button_new((Button *)self, x, y, width, height);
		self->f=&Render_f;

		self->text=render_text("Рендер!", 24, 0, &self->texture);
		self->render=false;
	}
	void Render_draw(Render *self)
	{
		if (self->hover||self->key)
		{
			if (self->pressed) glColor3ub(32, 0, 0);
			else glColor3ub(64, 32, 32);
		}
		else glColor3ub(64, 0, 0);
		square(self->x, self->y, self->width, self->height);
		if (self->hover||self->key)
		{
			if (self->pressed) glColor3ub(128, 0, 0);
			else glColor3ub(255, 128, 128);
		}
		else glColor3ub(255, 0, 0);
		box(self->x, self->y, self->width, self->height);
		glColor3ub(255, 255, 255);
		glBindTexture(GL_TEXTURE_2D, self->texture);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2i(0, 0); glVertex2i(self->x+(self->width-self->text->w)/2, self->y+(self->height-self->text->h)/2);
			glTexCoord2i(0, 1); glVertex2i(self->x+(self->width-self->text->w)/2, self->y+(self->height+self->text->h)/2);
			glTexCoord2i(1, 0); glVertex2i(self->x+(self->width+self->text->w)/2, self->y+(self->height-self->text->h)/2);
			glTexCoord2i(1, 1); glVertex2i(self->x+(self->width+self->text->w)/2, self->y+(self->height+self->text->h)/2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void Render_on_click(Render *self, Future_sound *song)
	{
		self->render=true;
	}
	void Render_remove(Render *self)
	{
		glDeleteTextures(1, &self->texture);
		SDL_FreeSurface(self->text);
	}
	typedef struct Ok Ok;
	struct Ok
	{
		Button;

		SDL_Surface *text;
		GLuint texture;
		bool ok;
	};
	void Ok_on_click(Ok *self, Future_sound *song);
	struct Ok Ok_f=
	{
		(void (*)(void *, Future_sound *))Ok_on_click
	};
	void Ok_new(Ok *self, GLint x, GLint y, GLint width, GLint height)
	{
		Button_new((Button *)self, x, y, width, height);
		self->f=&Ok_f;

		self->text=render_text("ОК!", 24, 0, &self->texture);
		self->ok=false;
	}
	void Ok_draw(Ok *self)
	{
		if (self->hover||self->key)
		{
			if (self->pressed) glColor3ub(32, 0, 0);
			else glColor3ub(64, 32, 32);
		}
		else glColor3ub(64, 0, 0);
		square(self->x, self->y, self->width, self->height);
		if (self->hover||self->key)
		{
			if (self->pressed) glColor3ub(128, 0, 0);
			else glColor3ub(255, 128, 128);
		}
		else glColor3ub(255, 0, 0);
		box(self->x, self->y, self->width, self->height);
		glColor3ub(255, 255, 255);
		glBindTexture(GL_TEXTURE_2D, self->texture);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2i(0, 0); glVertex2i(self->x+(self->width-self->text->w)/2, self->y+(self->height-self->text->h)/2);
			glTexCoord2i(0, 1); glVertex2i(self->x+(self->width-self->text->w)/2, self->y+(self->height+self->text->h)/2);
			glTexCoord2i(1, 0); glVertex2i(self->x+(self->width+self->text->w)/2, self->y+(self->height-self->text->h)/2);
			glTexCoord2i(1, 1); glVertex2i(self->x+(self->width+self->text->w)/2, self->y+(self->height+self->text->h)/2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void Ok_on_click(Ok *self, Future_sound *song)
	{
		self->ok=true;
	}
	void Ok_remove(Ok *self)
	{
		glDeleteTextures(1, &self->texture);
		SDL_FreeSurface(self->text);
	}
	typedef struct Cancel Cancel;
	struct Cancel
	{
		Button;

		SDL_Surface *text;
		GLuint texture;
		bool cancel;
	};
	void Cancel_on_click(Cancel *self, Future_sound *song);
	struct Cancel Cancel_f=
	{
		(void (*)(void *, Future_sound *))Cancel_on_click
	};
	void Cancel_new(Cancel *self, GLint x, GLint y, GLint width, GLint height)
	{
		Button_new((Button *)self, x, y, width, height);
		self->f=&Cancel_f;

		self->text=render_text("Отмена", 16, 0, &self->texture);
		self->cancel=false;
	}
	void Cancel_draw(Cancel *self)
	{
		Button_draw((Button *)self);
		glColor3ub(255, 255, 255);
		glBindTexture(GL_TEXTURE_2D, self->texture);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2i(0, 0); glVertex2i(self->x+(self->width-self->text->w)/2, self->y+(self->height-self->text->h)/2);
			glTexCoord2i(0, 1); glVertex2i(self->x+(self->width-self->text->w)/2, self->y+(self->height+self->text->h)/2);
			glTexCoord2i(1, 0); glVertex2i(self->x+(self->width+self->text->w)/2, self->y+(self->height-self->text->h)/2);
			glTexCoord2i(1, 1); glVertex2i(self->x+(self->width+self->text->w)/2, self->y+(self->height+self->text->h)/2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void Cancel_on_click(Cancel *self, Future_sound *song)
	{
		self->cancel=true;
	}
	void Cancel_remove(Cancel *self)
	{
		glDeleteTextures(1, &self->texture);
		SDL_FreeSurface(self->text);
	}
#endif

void audi(Future_sound *user_data, int16_t *stream, int length)
{
	#if defined(NOGUI)
		for (int a=0; a!=buffer_size; a++)
		{
			MDLRS_Modularis_update(&user_data->project);
			stream[2*a]=clamp(MDLRS_Modularis_get(&user_data->project, 0)*32768, -32768, 32767);
			stream[2*a+1]=clamp(MDLRS_Modularis_get(&user_data->project, 1)*32768, -32768, 32767);
		}
	#else
		uint32_t position=user_data->visualizer_position;
		user_data->visualizer_position=(position+buffer_size)%visualizer_buffer_size;
		user_data->loaded=true;
		SDL_LockMutex(user_data->mutex);
		for (int a=0; a!=buffer_size; a++)
		{
			MDLRS_Modularis_update(&user_data->project);
			stream[2*a]=clamp(MDLRS_Modularis_get(&user_data->project, 0)*32768, -32768, 32767);
			stream[2*a+1]=clamp(MDLRS_Modularis_get(&user_data->project, 1)*32768, -32768, 32767);
			user_data->visualizer_buffer[(position+a)%visualizer_buffer_size]=((int32_t)stream[2*a]+stream[2*a+1])/2;
		}
		SDL_UnlockMutex(user_data->mutex);
	#endif
}
void render_song(Future_sound *user_data)
{
	uint32_t song_size=sample_rate/(8*TEMPO)*4*60*SONG_LENGTH;
	uint32_t clipboard;
	FILE *file=fopen(output, "wb");
	fputs("RIFF", file); //chunkId
	clipboard=36+song_size; fwrite(&clipboard, 4, 1, file); //chunkSize
	fputs("WAVE", file); //format
	fputs("fmt ", file); //subchunk1Id
	clipboard=16; fwrite(&clipboard, 4, 1, file); //subchunk1Size
	clipboard=1; fwrite(&clipboard, 2, 1, file); //audioFormat
	clipboard=2; fwrite(&clipboard, 2, 1, file); //numChannels
	fwrite(&sample_rate, 4, 1, file); //sampleRate
	clipboard=4*sample_rate; fwrite(&clipboard, 4, 1, file); //byteRate
	clipboard=4; fwrite(&clipboard, 2, 1, file); //blockAlign
	clipboard=16; fwrite(&clipboard, 2, 1, file); //bitsPerSample
	fputs("data", file); //subchunk2Id
	fwrite(&song_size, 4, 1, file); //subchunk2Size
	//data
	uint32_t size_of_buffer=sizeof(int16_t)*2*buffer_size;
	int16_t *buffer=malloc(size_of_buffer);
	uint32_t a=0;
	while (true) if (a+size_of_buffer<song_size)
	{
		for (int a=0; a!=buffer_size; a++)
		{
			MDLRS_Modularis_update(&user_data->project);
			buffer[2*a]=clamp(MDLRS_Modularis_get(&user_data->project, 0)*32768, -32768, 32767);
			buffer[2*a+1]=clamp(MDLRS_Modularis_get(&user_data->project, 1)*32768, -32768, 32767);
		}
		fwrite(buffer, 1, size_of_buffer, file);
		a+=size_of_buffer;
	}
	else
	{
		int buffer_size=(song_size-a)/4;
		for (int a=0; a!=buffer_size; a++)
		{
			MDLRS_Modularis_update(&user_data->project);
			buffer[2*a]=clamp(MDLRS_Modularis_get(&user_data->project, 0)*32768, -32768, 32767);
			buffer[2*a+1]=clamp(MDLRS_Modularis_get(&user_data->project, 1)*32768, -32768, 32767);
		}
		fwrite(buffer, 1, song_size-a, file);
		break;
	}
	free(buffer);
	fclose(file);
	#if !defined(NOGUI)
		user_data->loaded=true;
	#endif
}

enum
{
	STATE_NONE,
	STATE_BUFFER,
	STATE_OUTPUT,
	STATE_SAMPLE_RATE,
	STATE_VISUALIZER_TIME
};

int main(int arguments_count, char **arguments)
{
	setlocale(LC_ALL, "Russian");
	int state=STATE_NONE;
	for (int argument=1; argument!=arguments_count; argument++) switch (state)
	{
		case STATE_NONE:
			if (*arguments[argument]=='-') switch (arguments[argument][1])
			{
				case '-':
					#if defined(NOGUI)||!defined(_WIN32)
						if (!strcmp(arguments[argument]+2, "help"))
						{
							printf(HELP);
							return 0;
						}
						else
					#endif
					if (strstr(arguments[argument]+2, "buffer=")) buffer_size=atoi(arguments[argument]+9);
					#if !(defined(NOGUI)||defined(_WIN32))
						else if (!strcmp(arguments[argument]+2, "nogui")) nogui=true;
					#endif
					else if (strstr(arguments[argument]+2, "output=")) output=arguments[argument]+9;
					else if (strstr(arguments[argument]+2, "sample-rate=")) sample_rate=atoi(arguments[argument]+14);
					#if !defined(NOGUI)
						else if (strstr(arguments[argument]+2, "visualizer-time=")) visualizer_time=atoi(arguments[argument]+18);
					#endif
					else fprintf(stderr, WARNING_UNKNOWN_OPTION, arguments[argument]);
					break;
				case 'b':
					if (arguments[argument][2]) buffer_size=atoi(arguments[argument]+2);
					else state=STATE_BUFFER;
					break;
				case 'o':
					if (arguments[argument][2]) output=arguments[argument]+2;
					else state=STATE_OUTPUT;
					break;
				case 'r':
					if (arguments[argument][2]) sample_rate=atoi(arguments[argument]+2);
					else state=STATE_SAMPLE_RATE;
					break;
				#if !defined(NOGUI)
					case 'v':
						if (arguments[argument][2]) visualizer_time=atoi(arguments[argument]+2);
						else state=STATE_VISUALIZER_TIME;
						break;
				#endif
				default:
					fprintf(stderr, WARNING_UNKNOWN_OPTION, arguments[argument]);
			}
			break;
		case STATE_BUFFER:
			buffer_size=atoi(arguments[argument]);
			state=STATE_NONE;
			break;
		case STATE_OUTPUT:
			output=arguments[argument];
			state=STATE_NONE;
			break;
		case STATE_SAMPLE_RATE:
			sample_rate=atoi(arguments[argument]);
			state=STATE_NONE;
			break;
		case STATE_VISUALIZER_TIME:
			visualizer_time=atoi(arguments[argument]);
			state=STATE_NONE;
	}
	visualizer_size=visualizer_time*sample_rate/1000;
	visualizer_buffer_size=visualizer_size+buffer_size;
	Future_sound song; Future_sound_new(&song);
	MDLRS_Discrete_key phase1={0, 32};
	MDLRS_Pattern_none pattern1; MDLRS_Pattern_none_new(&pattern1, 32);
	MDLRS_Note_key note1={0, 1, INTERPOLATION_NONE, true};
	MDLRS_Discrete_key phase2={0, 1};
	MDLRS_Continuous_key velocity1={1, 1, INTERPOLATION_NONE};
	MDLRS_Sequence sequence1={&note1, &phase2, &velocity1};
	MDLRS_Pattern pattern2; MDLRS_Pattern_new(&pattern2, 0, &sequence1, 1);
	MDLRS_Note_key note2={0, 32, INTERPOLATION_NONE, true};
	MDLRS_Discrete_key phases3[]={{0, 8}, {0, 8}, {0, 8}, {0, 8}};
	MDLRS_Continuous_key velocity2={1, 32, INTERPOLATION_NONE};
	MDLRS_Sequence sequence2={&note2, phases3, &velocity2};
	MDLRS_Pattern pattern3; MDLRS_Pattern_new(&pattern3, 32, &sequence2, 1);
	MDLRS_Pattern_none pattern4; MDLRS_Pattern_none_new(&pattern4, 256);
	MDLRS_Pattern_none pattern5; MDLRS_Pattern_none_new(&pattern5, 160);
	MDLRS_Note_key notes3[]=
	{
		{.duration=8, .pressed=false},
		{0, 24, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key phases4[]={{0, 24}, {0, 8}};
	MDLRS_Sequence sequence3={notes3, phases4, &velocity2};
	MDLRS_Pattern pattern6; MDLRS_Pattern_new(&pattern6, 32, &sequence3, 1);
	MDLRS_Pattern_none pattern7; MDLRS_Pattern_none_new(&pattern7, 288);
	MDLRS_Pattern_none pattern8; MDLRS_Pattern_none_new(&pattern8, 128);
	MDLRS_Discrete_key phase5={0, 128};
	MDLRS_Continuous_key velocity3={1, 128, INTERPOLATION_NONE};
	MDLRS_Pattern_none pattern9; MDLRS_Pattern_none_new(&pattern9, 416);
	MDLRS_Continuous_key velocity4={1, 256, INTERPOLATION_NONE};
	MDLRS_Pattern_none pattern10; MDLRS_Pattern_none_new(&pattern10, 800);
	MDLRS_Pattern_none pattern11; MDLRS_Pattern_none_new(&pattern11, 64);
	MDLRS_Discrete_key phases6[]={{0, 12}, {0, 8}, {0, 8}, {0, 8}, {0, 8}, {0, 8}, {0, 8}, {0, 4}};
	MDLRS_Pattern_none pattern12; MDLRS_Pattern_none_new(&pattern12, 16);
	MDLRS_Pattern_none pattern13; MDLRS_Pattern_none_new(&pattern13, 112);
	MDLRS_Note_key supersaw_bass_note1={-24, 32, INTERPOLATION_NONE, true};
	MDLRS_Continuous_key supersaw_bass_velocities1[]=
	{
		{0, 32, INTERPOLATION_SLOW},
		{1, 0, INTERPOLATION_NONE}
	};
	MDLRS_Sequence supersaw_bass_sequence1={&supersaw_bass_note1, &phase1, supersaw_bass_velocities1};
	MDLRS_Pattern supersaw_bass_pattern1; MDLRS_Pattern_new(&supersaw_bass_pattern1, 32, &supersaw_bass_sequence1, 1);
	MDLRS_Note_key supersaw_bass_notes2[]=
	{
		{-24, 2, INTERPOLATION_FAST, true},
		{-28, 62, INTERPOLATION_NONE, true},
		{-28, 2, INTERPOLATION_FAST, true},
		{-33, 62, INTERPOLATION_NONE, true},
		{-33, 2, INTERPOLATION_FAST, true},
		{-31, 62, INTERPOLATION_NONE, true},
		{-28, 32, INTERPOLATION_NONE, true},
		{-26, 16, INTERPOLATION_NONE, true},
		{.duration=16, .pressed=false}
	};
	MDLRS_Discrete_key supersaw_bass_phase2={0, 256};
	MDLRS_Sequence supersaw_bass_sequence2={supersaw_bass_notes2, &supersaw_bass_phase2, &velocity4};
	MDLRS_Pattern supersaw_bass_pattern2; MDLRS_Pattern_new(&supersaw_bass_pattern2, 256, &supersaw_bass_sequence2, 1);
	MDLRS_Note_key supersaw_bass_note3={-36, 128, INTERPOLATION_NONE, true};
	MDLRS_Sequence supersaw_bass_sequence3={&supersaw_bass_note3, &phase5, &velocity3};
	MDLRS_Pattern supersaw_bass_pattern3; MDLRS_Pattern_new(&supersaw_bass_pattern3, 128, &supersaw_bass_sequence3, 1);
	MDLRS_Note_key supersaw_bass_notes4[]=
	{
		{-36, 2, INTERPOLATION_FAST, true},
		{-40, 62, INTERPOLATION_NONE, true},
		{-40, 2, INTERPOLATION_FAST, true},
		{-41, 62, INTERPOLATION_NONE, true},
		{-41, 2, INTERPOLATION_FAST, true},
		{-43, 46, INTERPOLATION_NONE, true},
		{-43, 2, INTERPOLATION_FAST, true},
		{-38, 14, INTERPOLATION_NONE, true},
		{-38, 2, INTERPOLATION_FAST, true},
		{-33, 46, INTERPOLATION_NONE, true},
		{-33, 2, INTERPOLATION_FAST, true},
		{-34, 14, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence supersaw_bass_sequence4={supersaw_bass_notes4, &supersaw_bass_phase2, &velocity4};
	MDLRS_Pattern supersaw_bass_pattern4; MDLRS_Pattern_new(&supersaw_bass_pattern4, 256, &supersaw_bass_sequence4, 1);
	MDLRS_Note_key supersaw_bass_notes5[]=
	{
		{-34, 2, INTERPOLATION_FAST, true},
		{-40, 62, INTERPOLATION_NONE, true},
		{-40, 2, INTERPOLATION_FAST, true},
		{-41, 62, INTERPOLATION_NONE, true},
		{-41, 2, INTERPOLATION_FAST, true},
		{-43, 30, INTERPOLATION_NONE, true},
		{-43, 2, INTERPOLATION_FAST, true},
		{-38, 30, INTERPOLATION_NONE, true},
		{-38, 2, INTERPOLATION_FAST, true},
		{-33, 30, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence supersaw_bass_sequence5={supersaw_bass_notes5, &supersaw_bass_phase2, &velocity4};
	MDLRS_Pattern supersaw_bass_pattern5; MDLRS_Pattern_new(&supersaw_bass_pattern5, 224, &supersaw_bass_sequence5, 1);
	MDLRS_Pattern_none supersaw_bass_pattern6; MDLRS_Pattern_none_new(&supersaw_bass_pattern6, 528);
	MDLRS_Note_key supersaw_bass_notes6[]=
	{
		{-33, 48, INTERPOLATION_NONE, true},
		{-33, 4, INTERPOLATION_LINEAR, true},
		{-48, 12, .pressed=false}
	};
	MDLRS_Sequence supersaw_bass_sequence6={supersaw_bass_notes6, &supersaw_bass_phase2, &velocity4};
	MDLRS_Pattern supersaw_bass_pattern7; MDLRS_Pattern_new(&supersaw_bass_pattern7, 64, &supersaw_bass_sequence6, 1);
	MDLRS_Any_pattern *supersaw_bass_patterns_sequence1[]={(MDLRS_Any_pattern *)&supersaw_bass_pattern1, (MDLRS_Any_pattern *)&supersaw_bass_pattern2, (MDLRS_Any_pattern *)&pattern7, (MDLRS_Any_pattern *)&supersaw_bass_pattern3, (MDLRS_Any_pattern *)&supersaw_bass_pattern4, (MDLRS_Any_pattern *)&supersaw_bass_pattern5, (MDLRS_Any_pattern *)&supersaw_bass_pattern6, (MDLRS_Any_pattern *)&supersaw_bass_pattern7, NULL};
	MDLRS_Any_pattern **supersaw_bass_track=supersaw_bass_patterns_sequence1;
	MDLRS_Note_key jumpy_bass_notes1[]=
	{
		{.duration=4, .pressed=false},
		{-36+0, 1, INTERPOLATION_FAST, true},
		{-36, 7, INTERPOLATION_NONE, true},
		{-36+0, 1, INTERPOLATION_FAST, true},
		{-36, 7, INTERPOLATION_NONE, true},
		{-36+0, 1, INTERPOLATION_FAST, true},
		{-36, 7, INTERPOLATION_NONE, true},
		{-24+0, 1, INTERPOLATION_FAST, true},
		{-24, 7, INTERPOLATION_NONE, true},
		{-40+0, 1, INTERPOLATION_FAST, true},
		{-40, 7, INTERPOLATION_NONE, true},
		{-40+0, 1, INTERPOLATION_FAST, true},
		{-40, 7, INTERPOLATION_NONE, true},
		{-38+0, 1, INTERPOLATION_FAST, true},
		{-38, 7, INTERPOLATION_NONE, true},
		{-26+0, 1, INTERPOLATION_FAST, true},
		{-26, 3, INTERPOLATION_NONE, true}
	};
	MDLRS_Continuous_key jumpy_bass_velocity1={1, 64, INTERPOLATION_NONE};
	MDLRS_Sequence jumpy_bass_sequence1={jumpy_bass_notes1, phases6, &jumpy_bass_velocity1};
	MDLRS_Pattern jumpy_bass_pattern1; MDLRS_Pattern_new(&jumpy_bass_pattern1, 64, &jumpy_bass_sequence1, 1);
	MDLRS_Note_key jumpy_bass_notes2[]=
	{
		{.duration=4, .pressed=false},
		{-36+0, 1, INTERPOLATION_FAST, true},
		{-36, 7, INTERPOLATION_NONE, true},
		{-36+0, 1, INTERPOLATION_FAST, true},
		{-36, 7, INTERPOLATION_NONE, true},
		{-36+0, 1, INTERPOLATION_FAST, true},
		{-36, 7, INTERPOLATION_NONE, true},
		{-24+0, 1, INTERPOLATION_FAST, true},
		{-24, 7, INTERPOLATION_NONE, true},
		{-40+0, 1, INTERPOLATION_FAST, true},
		{-40, 7, INTERPOLATION_NONE, true},
		{-40+0, 1, INTERPOLATION_FAST, true},
		{-40, 7, INTERPOLATION_NONE, true},
		{-31+0, 1, INTERPOLATION_FAST, true},
		{-31, 7, INTERPOLATION_NONE, true},
		{-19+0, 1, INTERPOLATION_FAST, true},
		{-19, 3, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence jumpy_bass_sequence2={jumpy_bass_notes2, phases6, &jumpy_bass_velocity1};
	MDLRS_Pattern jumpy_bass_pattern2; MDLRS_Pattern_new(&jumpy_bass_pattern2, 64, &jumpy_bass_sequence2, 1);
	MDLRS_Note_key jumpy_bass_notes3[]=
	{
		{-40, 4, INTERPOLATION_NONE, true},
		{-28, 4, INTERPOLATION_NONE, true},
		{-40, 8, INTERPOLATION_NONE, true},
		{-26, 4, INTERPOLATION_NONE, true},
		{-38, 8, INTERPOLATION_NONE, true},
		{-14, 4, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key jumpy_bass_phases1[]={{0, 4}, {0, 2}, {0, 2}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}};
	MDLRS_Sequence jumpy_bass_sequence3={jumpy_bass_notes3, jumpy_bass_phases1, &velocity2};
	MDLRS_Pattern jumpy_bass_pattern3; MDLRS_Pattern_new(&jumpy_bass_pattern3, 32, &jumpy_bass_sequence3, 1);
	MDLRS_Note_key jumpy_bass_notes4[]=
	{
		{-36, 4, INTERPOLATION_NONE, true},
		{-24, 8, INTERPOLATION_NONE, true},
		{-36, 4, INTERPOLATION_NONE, true},
		{-26, 4, INTERPOLATION_NONE, true},
		{-38, 4, INTERPOLATION_NONE, true},
		{-26, 4, INTERPOLATION_NONE, true},
		{-38, 4, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence jumpy_bass_sequence4={jumpy_bass_notes4, jumpy_bass_phases1, &velocity2};
	MDLRS_Pattern jumpy_bass_pattern4; MDLRS_Pattern_new(&jumpy_bass_pattern4, 32, &jumpy_bass_sequence4, 1);
	MDLRS_Note_key jumpy_bass_notes5[]=
	{
		{-36, 4, INTERPOLATION_NONE, true},
		{-24, 8, INTERPOLATION_NONE, true},
		{-36, 4, INTERPOLATION_NONE, true},
		{-21, 4, INTERPOLATION_NONE, true},
		{-14, 4, INTERPOLATION_NONE, true},
		{-9, 4, INTERPOLATION_NONE, true},
		{-26, 4, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence jumpy_bass_sequence5={jumpy_bass_notes5, jumpy_bass_phases1, &velocity2};
	MDLRS_Pattern jumpy_bass_pattern5; MDLRS_Pattern_new(&jumpy_bass_pattern5, 32, &jumpy_bass_sequence5, 1);
	MDLRS_Note_key jumpy_bass_notes6[]=
	{
		{-40, 16, INTERPOLATION_NONE, true},
		{-38, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key jumpy_bass_phases2[]={{0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 8}};
	MDLRS_Sequence jumpy_bass_sequence6={jumpy_bass_notes6, jumpy_bass_phases2, &velocity2};
	MDLRS_Pattern jumpy_bass_pattern6; MDLRS_Pattern_new(&jumpy_bass_pattern6, 32, &jumpy_bass_sequence6, 1);
	MDLRS_Note_key jumpy_bass_note7={-36, 32, INTERPOLATION_NONE, true};
	MDLRS_Discrete_key jumpy_bass_phases3[]={{0, 8}, {0, 4}, {0, 8}, {0, 12}};
	MDLRS_Sequence jumpy_bass_sequence7={&jumpy_bass_note7, jumpy_bass_phases3, &velocity2};
	MDLRS_Pattern jumpy_bass_pattern7; MDLRS_Pattern_new(&jumpy_bass_pattern7, 32, &jumpy_bass_sequence7, 1);
	MDLRS_Pattern_none jumpy_bass_pattern8; MDLRS_Pattern_none_new(&jumpy_bass_pattern8, 528);
	MDLRS_Note_key jumpy_bass_notes8[]=
	{
		{-28, 16, INTERPOLATION_NONE, true},
		{-26, 16, INTERPOLATION_NONE, true},
		{-24, 16, INTERPOLATION_NONE, true},
		{-29, 16, INTERPOLATION_NONE, true},
		{-28, 16, INTERPOLATION_NONE, true},
		{-26, 16, INTERPOLATION_NONE, true},
		{-24, 16, INTERPOLATION_NONE, true},
		{-21, 16, INTERPOLATION_NONE, true},
		{-28, 16, INTERPOLATION_NONE, true},
		{-26, 16, INTERPOLATION_NONE, true},
		{-24, 16, INTERPOLATION_NONE, true},
		{-26, 16, INTERPOLATION_NONE, true},
		{-28, 16, INTERPOLATION_NONE, true},
		{-26, 16, INTERPOLATION_NONE, true}
	};	
	MDLRS_Discrete_key jumpy_bass_phases4[56];
	for (int a=0; a!=56; a++) jumpy_bass_phases4[a]=(MDLRS_Discrete_key){0, 4};
	MDLRS_Sequence jumpy_bass_sequence8={jumpy_bass_notes8, jumpy_bass_phases4, &velocity4};
	MDLRS_Pattern jumpy_bass_pattern9; MDLRS_Pattern_new(&jumpy_bass_pattern9, 224, &jumpy_bass_sequence8, 1);
	MDLRS_Pattern_none jumpy_bass_pattern10; MDLRS_Pattern_none_new(&jumpy_bass_pattern10, 80);
	MDLRS_Any_pattern *jumpy_bass_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern7, (MDLRS_Any_pattern *)&jumpy_bass_pattern1, (MDLRS_Any_pattern *)&jumpy_bass_pattern2, (MDLRS_Any_pattern *)&jumpy_bass_pattern3, (MDLRS_Any_pattern *)&jumpy_bass_pattern4, (MDLRS_Any_pattern *)&jumpy_bass_pattern3, (MDLRS_Any_pattern *)&jumpy_bass_pattern5, (MDLRS_Any_pattern *)&jumpy_bass_pattern6, (MDLRS_Any_pattern *)&pattern12, (MDLRS_Any_pattern *)&jumpy_bass_pattern7, (MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&jumpy_bass_pattern7, (MDLRS_Any_pattern *)&jumpy_bass_pattern8, (MDLRS_Any_pattern *)&jumpy_bass_pattern1, (MDLRS_Any_pattern *)&jumpy_bass_pattern1, (MDLRS_Any_pattern *)&jumpy_bass_pattern1, (MDLRS_Any_pattern *)&jumpy_bass_pattern2, (MDLRS_Any_pattern *)&jumpy_bass_pattern9, (MDLRS_Any_pattern *)&jumpy_bass_pattern10, NULL};
	MDLRS_Any_pattern **jumpy_bass_track=jumpy_bass_patterns_sequence1;
	MDLRS_Note_key piano_notes1[]=
	{
		{.duration=4, .pressed=false},
		{-2, 4, INTERPOLATION_NONE, true},
		{2, 4, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{7, 16, INTERPOLATION_NONE, true},
		{8, 16, INTERPOLATION_NONE, true},
		{10, 20, INTERPOLATION_NONE, true},
		{-2, 4, INTERPOLATION_NONE, true},
		{2, 4, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{7, 16, INTERPOLATION_NONE, true},
		{5, 16, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{2, 4, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{0, 4, INTERPOLATION_NONE, true},
		{2, 4, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{7, 8, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{5, 4, INTERPOLATION_NONE, true},
		{3, 12, INTERPOLATION_NONE, true},
		{0, 4, INTERPOLATION_NONE, true},
		{2, 4, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{10, 8, INTERPOLATION_NONE, true},
		{12, 24, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key piano_phases1[]={{0, 8}, {0, 4}, {0, 4}, {0, 16}, {0, 16}, {0, 20}, {0, 4}, {0, 4}, {0, 4}, {0, 16}, {0, 16}, {0, 8}, {0, 4}, {0, 8}, {0, 4}, {0, 4}, {0, 4}, {0, 8}, {0, 8}, {0, 8}, {0, 8}, {0, 4}, {0, 4}, {0, 12}, {0, 4}, {0, 4}, {0, 4}, {0, 8}, {0, 8}, {0, 8}, {0, 24}};
	MDLRS_Sequence piano_sequence1={piano_notes1, piano_phases1, &velocity4};
	MDLRS_Pattern piano_pattern1; MDLRS_Pattern_new(&piano_pattern1, 256, &piano_sequence1, 1);
	MDLRS_Note_key piano_notes2[]=
	{
		{2, 12, INTERPOLATION_NONE, true},
		{3, 12, INTERPOLATION_NONE, true},
		{5, 12, INTERPOLATION_NONE, true},
		{3, 12, INTERPOLATION_NONE, true},
		{5, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{10, 4, INTERPOLATION_NONE, true},
		{14, 4, INTERPOLATION_NONE, true},
		{2, 12, INTERPOLATION_NONE, true},
		{3, 12, INTERPOLATION_NONE, true},
		{5, 12, INTERPOLATION_NONE, true},
		{7, 12, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{2, 12, INTERPOLATION_NONE, true},
		{3, 12, INTERPOLATION_NONE, true},
		{5, 12, INTERPOLATION_NONE, true},
		{7, 12, INTERPOLATION_NONE, true},
		{5, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{10, 4, INTERPOLATION_NONE, true},
		{14, 4, INTERPOLATION_NONE, true},
		{15, 12, INTERPOLATION_NONE, true},
		{14, 12, INTERPOLATION_NONE, true},
		{10, 16, INTERPOLATION_NONE, true},
		{12, 8, INTERPOLATION_NONE, true},
		{10, 8, INTERPOLATION_NONE, true},
		{7, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key piano_notes3[]=
	{
		{0, 20, INTERPOLATION_NONE, true},
		{-4, 44, INTERPOLATION_NONE, true},
		{-2, 48, INTERPOLATION_NONE, true},
		{2, 16, INTERPOLATION_NONE, true},
		{0, 64, INTERPOLATION_NONE, true},
		{10, 24, INTERPOLATION_NONE, true},
		{7, 40, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key piano_notes4[]=
	{
		{-4, 64, INTERPOLATION_NONE, true},
		{-5, 128, INTERPOLATION_NONE, true},
		{7, 64, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key piano_phases2[]={{0, 12}, {0, 12}, {0, 12}, {0, 12}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 12}, {0, 12}, {0, 12}, {0, 12}, {0, 8}, {0, 8}, {0, 12}, {0, 12}, {0, 12}, {0, 12}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 12}, {0, 12}, {0, 8}, {0, 8}, {0, 8}, {0, 8}, {0, 8}};
	MDLRS_Discrete_key piano_phases3[]={{0, 20}, {0, 44}, {0, 20}, {0, 28}, {0, 16}, {0, 12}, {0, 52}, {0, 12}, {0, 12}, {0, 40}};
	MDLRS_Discrete_key piano_phases4[]={{0, 64}, {0, 64}, {0, 64}, {0, 64}};
	MDLRS_Sequence piano_sequences2[]=
	{
		{piano_notes2, piano_phases2, &velocity4},
		{piano_notes3, piano_phases3, &velocity4},
		{piano_notes4, piano_phases4, &velocity4}
	};
	MDLRS_Pattern piano_pattern2; MDLRS_Pattern_new(&piano_pattern2, 256, piano_sequences2, 3);
	MDLRS_Pattern piano_pattern3; MDLRS_Pattern_new(&piano_pattern3, 224, piano_sequences2, 3);
	MDLRS_Note_key piano_notes5[]=
	{
		{12, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{12, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{12, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{12, 4, INTERPOLATION_NONE, true},
		{10, 4, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key piano_phases5[]={{0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}};
	MDLRS_Continuous_key piano_velocity1={.75f, 68, INTERPOLATION_NONE};
	MDLRS_Sequence piano_sequence3={piano_notes5, piano_phases5, &piano_velocity1};
	MDLRS_Pattern piano_pattern4; MDLRS_Pattern_new(&piano_pattern4, 44, &piano_sequence3, 1);
	MDLRS_Note_key piano_notes6[]=
	{
		{7, 4, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key piano_phases6[]={{0, 4}, {0, 8}, {0, 8}};
	MDLRS_Sequence piano_sequence4={piano_notes6, piano_phases6, &piano_velocity1};
	MDLRS_Pattern piano_pattern5; MDLRS_Pattern_new(&piano_pattern5, 20, &piano_sequence4, 1);
	MDLRS_Note_key piano_notes7[]=
	{
		{12, 4, INTERPOLATION_NONE, true},
		{15, 8, INTERPOLATION_NONE, true},
		{10, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence piano_sequence5={piano_notes7, piano_phases6, &piano_velocity1};
	MDLRS_Pattern piano_pattern6; MDLRS_Pattern_new(&piano_pattern6, 20, &piano_sequence5, 1);
	MDLRS_Pattern_none piano_pattern7; MDLRS_Pattern_none_new(&piano_pattern7, 1404);
	MDLRS_Note_key piano_notes8[]=
	{
		{5, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{5, 4, INTERPOLATION_NONE, true},
		{3, 12, INTERPOLATION_NONE, true},
		{5, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{5, 4, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{12, 4, INTERPOLATION_NONE, true},
		{10, 8, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key piano_phases7[]={{0, 4}, {0, 4}, {0, 4}, {0, 8}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 8}, {0, 4}, {0, 4}, {0, 8}, {0, 8}};
	MDLRS_Sequence piano_sequence6={piano_notes8, piano_phases7, &piano_velocity1};
	MDLRS_Pattern piano_pattern8; MDLRS_Pattern_new(&piano_pattern8, 68, &piano_sequence6, 1);
	MDLRS_Pattern_none piano_pattern9; MDLRS_Pattern_none_new(&piano_pattern9, 304);
	MDLRS_Note_key piano_notes9[]=
	{
		{.duration=6, .pressed=false},
		{10, 5, INTERPOLATION_NONE, true},
		{12, 3, INTERPOLATION_NONE, true},
		{10, 3, INTERPOLATION_NONE, true},
		{12, 2, INTERPOLATION_NONE, true},
		{10, 2, INTERPOLATION_NONE, true},
		{12, 2, INTERPOLATION_NONE, true},
		{10, 2, INTERPOLATION_NONE, true},
		{12, 2, INTERPOLATION_NONE, true},
		{10, 2, INTERPOLATION_NONE, true},
		{12, 2, INTERPOLATION_NONE, true},
		{10, 2, INTERPOLATION_NONE, true},
		{12, 2, INTERPOLATION_NONE, true},
		{10, 13, INTERPOLATION_NONE, true},
		{7, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key piano_notes10[]=
	{
		{.duration=48, .pressed=false},
		{3, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key piano_notes11[]=
	{
		{.duration=48, .pressed=false},
		{-2, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key piano_phases8[]={{0, 11}, {0, 3}, {0, 3}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 13}, {0, 16}};
	MDLRS_Sequence piano_sequences7[]=
	{
		{piano_notes9, piano_phases8, &velocity3},
		{piano_notes10, &phase5, &velocity3},
		{piano_notes11, &phase5, &velocity3}
	};
	MDLRS_Pattern piano_pattern10; MDLRS_Pattern_new(&piano_pattern10, 64, piano_sequences7, 3);
	MDLRS_Any_pattern *piano_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&piano_pattern1, (MDLRS_Any_pattern *)&pattern9, (MDLRS_Any_pattern *)&piano_pattern2, (MDLRS_Any_pattern *)&piano_pattern3, (MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&piano_pattern4, (MDLRS_Any_pattern *)&piano_pattern5, (MDLRS_Any_pattern *)&piano_pattern4, (MDLRS_Any_pattern *)&piano_pattern6, (MDLRS_Any_pattern *)&piano_pattern4, (MDLRS_Any_pattern *)&piano_pattern5, (MDLRS_Any_pattern *)&piano_pattern9, (MDLRS_Any_pattern *)&piano_pattern10, NULL};
	MDLRS_Any_pattern *piano_patterns_sequence2[]={(MDLRS_Any_pattern *)&piano_pattern7, (MDLRS_Any_pattern *)&piano_pattern8, (MDLRS_Any_pattern *)&piano_pattern9, NULL};
	MDLRS_Any_pattern **piano_tracks[]={piano_patterns_sequence1, piano_patterns_sequence2};
	MDLRS_Note_key chords_notes1[]=
	{
		{3, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{7, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{7, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{8, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{8, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{10, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{10, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{10, 6, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes2[]=
	{
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 6, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes3[]=
	{
		{-5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{2, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{2, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{2, 6, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key chords_phase1={.25f, 64};
	MDLRS_Continuous_key chords_velocity1={1, 64, INTERPOLATION_NONE};
	MDLRS_Sequence chords_sequences1[]=
	{
		{chords_notes1, &chords_phase1, &chords_velocity1},
		{chords_notes2, &chords_phase1, &chords_velocity1},
		{chords_notes3, &chords_phase1, &chords_velocity1}
	};
	MDLRS_Pattern chords_pattern1; MDLRS_Pattern_new(&chords_pattern1, 64, chords_sequences1, 3);
	MDLRS_Note_key chords_notes4[]=
	{
		{3, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{7, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{7, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{8, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{8, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{8, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes5[]=
	{
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes6[]=
	{
		{-5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 5, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-4, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-4, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-4, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence chords_sequences2[]=
	{
		{chords_notes4, &chords_phase1, &chords_velocity1},
		{chords_notes5, &chords_phase1, &chords_velocity1},
		{chords_notes6, &chords_phase1, &chords_velocity1}
	};
	MDLRS_Pattern chords_pattern2; MDLRS_Pattern_new(&chords_pattern2, 64, chords_sequences2, 3);
	MDLRS_Note_key chords_notes7[]=
	{
		{0, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{2, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false}
	};
	MDLRS_Note_key chords_notes8[]=
	{
		{-4, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-4, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-2, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{2, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false}
	};
	MDLRS_Note_key chords_notes9[]=
	{
		{-9, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-9, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-7, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-2, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false}
	};
	MDLRS_Sequence chords_sequences3[]=
	{
		{chords_notes7, &chords_phase1, &velocity2},
		{chords_notes8, &chords_phase1, &velocity2},
		{chords_notes9, &chords_phase1, &velocity2}
	};
	MDLRS_Pattern chords_pattern3; MDLRS_Pattern_new(&chords_pattern3, 32, chords_sequences3, 3);
	MDLRS_Note_key chords_notes10[]=
	{
		{3, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{2, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-2, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false}
	};
	MDLRS_Note_key chords_notes11[]=
	{
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-2, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-7, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false}
	};
	MDLRS_Note_key chords_notes12[]=
	{
		{-5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-7, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-10, 7, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false}
	};
	MDLRS_Sequence chords_sequences4[]=
	{
		{chords_notes10, &chords_phase1, &velocity2},
		{chords_notes11, &chords_phase1, &velocity2},
		{chords_notes12, &chords_phase1, &velocity2}
	};
	MDLRS_Pattern chords_pattern4; MDLRS_Pattern_new(&chords_pattern4, 32, chords_sequences4, 3);
	MDLRS_Note_key chords_notes13[]=
	{
		{3, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{2, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{2, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{7, 8, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes14[]=
	{
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{3, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes15[]=
	{
		{-5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-2, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence chords_sequences5[]=
	{
		{chords_notes13, &chords_phase1, &velocity2},
		{chords_notes14, &chords_phase1, &velocity2},
		{chords_notes15, &chords_phase1, &velocity2}
	};
	MDLRS_Pattern chords_pattern5; MDLRS_Pattern_new(&chords_pattern5, 32, chords_sequences5, 3);
	MDLRS_Note_key chords_notes16[]=
	{
		{0, 64, INTERPOLATION_NONE, true},
		{2, 64, INTERPOLATION_NONE, true},
		{3, 32, INTERPOLATION_NONE, true},
		{5, 32, INTERPOLATION_NONE, true},
		{7, 32, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes17[]=
	{
		{-4, 64, INTERPOLATION_NONE, true},
		{-2, 64, INTERPOLATION_NONE, true},
		{0, 64, INTERPOLATION_NONE, true},
		{3, 32, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes18[]=
	{
		{-7, 64, INTERPOLATION_NONE, true},
		{-5, 64, INTERPOLATION_NONE, true},
		{-4, 64, INTERPOLATION_NONE, true},
		{-2, 32, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes19[]=
	{
		{-9, 128, INTERPOLATION_NONE, true},
		{-7, 32, INTERPOLATION_NONE, true},
		{-9, 32, INTERPOLATION_NONE, true},
		{-4, 32, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key chords_phases2[]={{.25f, 64}, {.25f, 64}, {.25f, 32}, {.25f, 32}, {.25f, 32}};
	MDLRS_Discrete_key chords_phases3[]={{.25f, 64}, {.25f, 64}, {.25f, 64}, {.25f, 32}};
	MDLRS_Continuous_key chords_velocities2[]=
	{
		{.5f, 192, INTERPOLATION_LINEAR},
		{1, 32, INTERPOLATION_NONE}
	};
	MDLRS_Sequence chords_sequences6[]=
	{
		{chords_notes16, chords_phases2, chords_velocities2},
		{chords_notes17, chords_phases3, chords_velocities2},
		{chords_notes18, chords_phases3, chords_velocities2},
		{chords_notes19, chords_phases2, chords_velocities2}
	};
	MDLRS_Pattern chords_pattern6; MDLRS_Pattern_new(&chords_pattern6, 224, chords_sequences6, 4);
	MDLRS_Note_key chords_notes20[]=
	{
		{3, 48, INTERPOLATION_NONE, true},
		{5, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes21[]=
	{
		{0, 48, INTERPOLATION_NONE, true},
		{2, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes22[]=
	{
		{-5, 32, INTERPOLATION_NONE, true},
		{-4, 16, INTERPOLATION_NONE, true},
		{-2, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key chords_phases4[]={{.25f, 32}, {.25f, 16}, {.25f, 16}};
	MDLRS_Sequence chords_sequences7[]=
	{
		{chords_notes20, chords_phases4, &chords_velocity1},
		{chords_notes21, chords_phases4, &chords_velocity1},
		{chords_notes22, chords_phases4, &chords_velocity1}
	};
	MDLRS_Pattern chords_pattern7; MDLRS_Pattern_new(&chords_pattern7, 64, chords_sequences7, 3);
	MDLRS_Note_key chords_notes23[]=
	{
		{7, 32, INTERPOLATION_NONE, true},
		{8, 16, INTERPOLATION_NONE, true},
		{10, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key chords_notes24[]=
	{
		{2, 32, INTERPOLATION_NONE, true},
		{3, 16, INTERPOLATION_NONE, true},
		{5, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence chords_sequences8[]=
	{
		{chords_notes23, chords_phases4, &chords_velocity1},
		{chords_notes20, chords_phases4, &chords_velocity1},
		{chords_notes21, chords_phases4, &chords_velocity1},
		{chords_notes24, chords_phases4, &chords_velocity1}
	};
	MDLRS_Pattern chords_pattern8; MDLRS_Pattern_new(&chords_pattern8, 64, chords_sequences8, 4);
	MDLRS_Note_key chords_notes25[]=
	{
		{7, 32, INTERPOLATION_NONE, true},
		{8, 16, INTERPOLATION_NONE, true},
		{5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{5, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false}
	};
	MDLRS_Note_key chords_notes26[]=
	{
		{3, 48, INTERPOLATION_NONE, true},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{0, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false}
	};
	MDLRS_Note_key chords_notes27[]=
	{
		{0, 48, INTERPOLATION_NONE, true},
		{-4, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-4, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-4, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false},
		{-4, 3, INTERPOLATION_NONE, true},
		{.duration=1, .pressed=false}
	};
	MDLRS_Note_key chords_notes28[]=
	{
		{2, 32, INTERPOLATION_NONE, true},
		{3, 16, INTERPOLATION_NONE, true},
		{.duration=16, .pressed=false}
	};
	MDLRS_Sequence chords_sequences9[]=
	{
		{chords_notes25, chords_phases4, &chords_velocity1},
		{chords_notes26, chords_phases4, &chords_velocity1},
		{chords_notes27, chords_phases4, &chords_velocity1},
		{chords_notes28, chords_phases4, &chords_velocity1}
	};
	MDLRS_Pattern chords_pattern9; MDLRS_Pattern_new(&chords_pattern9, 64, chords_sequences9, 4);
	MDLRS_Note_key chords_notes29[]=
	{
		{7, 48, INTERPOLATION_NONE, true},
		{.duration=16, .pressed=false}
	};
	MDLRS_Note_key chords_notes30[]=
	{
		{3, 48, INTERPOLATION_NONE, true},
		{.duration=16, .pressed=false}
	};
	MDLRS_Note_key chords_notes31[]=
	{
		{-2, 48, INTERPOLATION_NONE, true},
		{.duration=16, .pressed=false}
	};
	MDLRS_Sequence chords_sequences10[]=
	{
		{chords_notes29, &chords_phase1, &chords_velocity1},
		{chords_notes30, &chords_phase1, &chords_velocity1},
		{chords_notes31, &chords_phase1, &chords_velocity1}
	};
	MDLRS_Pattern chords_pattern10; MDLRS_Pattern_new(&chords_pattern10, 64, chords_sequences10, 3);
	MDLRS_Any_pattern *chords_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern7, (MDLRS_Any_pattern *)&chords_pattern1, (MDLRS_Any_pattern *)&chords_pattern2, (MDLRS_Any_pattern *)&chords_pattern3, (MDLRS_Any_pattern *)&chords_pattern4, (MDLRS_Any_pattern *)&chords_pattern3, (MDLRS_Any_pattern *)&chords_pattern5, (MDLRS_Any_pattern *)&pattern9, (MDLRS_Any_pattern *)&chords_pattern6, (MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&chords_pattern7, (MDLRS_Any_pattern *)&chords_pattern8, (MDLRS_Any_pattern *)&chords_pattern7, (MDLRS_Any_pattern *)&chords_pattern9, (MDLRS_Any_pattern *)&chords_pattern3, (MDLRS_Any_pattern *)&chords_pattern4, (MDLRS_Any_pattern *)&chords_pattern3, (MDLRS_Any_pattern *)&chords_pattern5, (MDLRS_Any_pattern *)&chords_pattern3, (MDLRS_Any_pattern *)&chords_pattern4, (MDLRS_Any_pattern *)&chords_pattern3, (MDLRS_Any_pattern *)&pattern12, (MDLRS_Any_pattern *)&chords_pattern10, NULL};
	MDLRS_Any_pattern **chords_track=chords_patterns_sequence1;
	MDLRS_Note_key main_lead_single_notes1[]=
	{
		{.duration=16, .pressed=false},
		{15, 4, INTERPOLATION_NONE, true},
		{17, 4, INTERPOLATION_NONE, true},
		{19, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key main_lead_single_phases1[]={{0, 20}, {0, 4}, {0, 8}};
	MDLRS_Sequence main_lead_single_sequence1={main_lead_single_notes1, main_lead_single_phases1, &velocity2};
	MDLRS_Pattern main_lead_single_pattern1; MDLRS_Pattern_new(&main_lead_single_pattern1, 32, &main_lead_single_sequence1, 1);
	MDLRS_Note_key main_lead_single_notes2[]=
	{
		{2, 4, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{5, 4, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{5, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key main_lead_single_phases2[]={{0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 8}};
	MDLRS_Sequence main_lead_single_sequence2={main_lead_single_notes2, main_lead_single_phases2, &velocity2};
	MDLRS_Pattern main_lead_single_pattern2; MDLRS_Pattern_new(&main_lead_single_pattern2, 32, &main_lead_single_sequence2, 1);
	MDLRS_Pattern_none main_lead_single_pattern3; MDLRS_Pattern_none_new(&main_lead_single_pattern3, 1200);
	MDLRS_Any_pattern *main_lead_single_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern4, (MDLRS_Any_pattern *)&main_lead_single_pattern1, (MDLRS_Any_pattern *)&pattern4, (MDLRS_Any_pattern *)&main_lead_single_pattern2, (MDLRS_Any_pattern *)&main_lead_single_pattern3, NULL};
	MDLRS_Any_pattern **main_lead_single_track=main_lead_single_patterns_sequence1;
	MDLRS_Note_key main_lead_notes1[]=
	{
		{10, 4, INTERPOLATION_NONE, true},
		{12, 4, INTERPOLATION_NONE, true},
		{10, 4, INTERPOLATION_NONE, true},
		{7, 8, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{7, 8, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{10, 4, INTERPOLATION_NONE, true},
		{7, 8, INTERPOLATION_NONE, true},
		{10, 4, INTERPOLATION_NONE, true},
		{3, 4, INTERPOLATION_NONE, true},
		{10, 4, INTERPOLATION_NONE, true},
		{7, 8, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{7, 8, INTERPOLATION_NONE, true},
		{8, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key main_lead_phases1[]={{0, 4}, {0, 4}, {0, 4}, {0, 8}, {0, 8}, {0, 8}, {0, 8}, {0, 8}, {0, 4}, {0, 8}, {0, 4}, {0, 4}, {0, 4}, {0, 8}, {0, 8}, {0, 8}, {0, 8}, {0, 8}};
	MDLRS_Sequence main_lead_sequence1={main_lead_notes1, main_lead_phases1, &velocity3};
	MDLRS_Pattern main_lead_pattern1; MDLRS_Pattern_new(&main_lead_pattern1, 116, &main_lead_sequence1, 1);
	MDLRS_Note_key main_lead_notes2[]=
	{
		{5, 4, INTERPOLATION_NONE, true},
		{12, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key main_lead_phases2[]={{0, 4}, {0, 4}, {0, 4}};
	MDLRS_Continuous_key main_lead_velocity1={1, 12, INTERPOLATION_NONE};
	MDLRS_Sequence main_lead_sequence2={main_lead_notes2, main_lead_phases2, &main_lead_velocity1};
	MDLRS_Pattern main_lead_pattern2; MDLRS_Pattern_new(&main_lead_pattern2, 12, &main_lead_sequence2, 1);
	MDLRS_Note_key main_lead_notes3[]=
	{
		{7, 4, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key main_lead_phases3[]={{0, 4}, {0, 8}};
	MDLRS_Sequence main_lead_sequence3={main_lead_notes3, main_lead_phases3, &main_lead_velocity1};
	MDLRS_Pattern main_lead_pattern3; MDLRS_Pattern_new(&main_lead_pattern3, 12, &main_lead_sequence3, 1);
	MDLRS_Note_key main_lead_notes4[]=
	{
		{12, 4, INTERPOLATION_NONE, true},
		{10, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{10, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{3, 8, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{10, 4, INTERPOLATION_NONE, true},
		{7, 4, INTERPOLATION_NONE, true},
		{5, 8, INTERPOLATION_NONE, true},
		{7, 8, INTERPOLATION_NONE, true},
		{8, 8, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key main_lead_phases4[]={{0, 4}, {0, 4}, {0, 4}, {0, 8}, {0, 8}, {0, 8}, {0, 8}, {0, 8}, {0, 4}, {0, 4}, {0, 8}, {0, 8}, {0, 8}, {0, 4}, {0, 4}, {0, 8}, {0, 8}, {0, 8}};
	MDLRS_Sequence main_lead_sequence4={main_lead_notes4, main_lead_phases4, &velocity3};
	MDLRS_Pattern main_lead_pattern4; MDLRS_Pattern_new(&main_lead_pattern4, 116, &main_lead_sequence4, 1);
	MDLRS_Pattern_none main_lead_pattern5; MDLRS_Pattern_none_new(&main_lead_pattern5, 304);
	MDLRS_Any_pattern *main_lead_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern7, (MDLRS_Any_pattern *)&main_lead_pattern1, (MDLRS_Any_pattern *)&main_lead_pattern2, (MDLRS_Any_pattern *)&pattern10, (MDLRS_Any_pattern *)&main_lead_pattern1, (MDLRS_Any_pattern *)&main_lead_pattern3, (MDLRS_Any_pattern *)&main_lead_pattern4, (MDLRS_Any_pattern *)&main_lead_pattern2, (MDLRS_Any_pattern *)&main_lead_pattern5, NULL};
	MDLRS_Any_pattern **main_lead_track=main_lead_patterns_sequence1;
	MDLRS_Note_key another_lead_notes1[]=
	{
		{3, 1, INTERPOLATION_FAST, true},
		{8, 3, INTERPOLATION_NONE, true},
		{8, 1, INTERPOLATION_FAST, true},
		{7, 3, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{5, 3, INTERPOLATION_NONE, true},
		{5, 1, INTERPOLATION_FAST, true},
		{10, 3, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{7, 3, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{5, 3, INTERPOLATION_NONE, true},
		{5, 1, INTERPOLATION_FAST, true},
		{10, 1, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{7, 1, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{10, 1, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{7, 1, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{12, 3, INTERPOLATION_NONE, true},
		{12, 1, INTERPOLATION_FAST, true},
		{10, 3, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{7, 3, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{10, 3, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{7, 3, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{5, 3, INTERPOLATION_NONE, true},
		{5, 1, INTERPOLATION_FAST, true},
		{7, 1, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{5, 1, INTERPOLATION_NONE, true},
		{5, 1, INTERPOLATION_FAST, true},
		{7, 1, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{5, 1, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence another_lead_sequence1={another_lead_notes1, &phase5, &velocity3};
	MDLRS_Pattern another_lead_pattern1; MDLRS_Pattern_new(&another_lead_pattern1, 64, &another_lead_sequence1, 1);
	MDLRS_Note_key another_lead_notes2[]=
	{
		{5, 1, INTERPOLATION_FAST, true},
		{8, 3, INTERPOLATION_NONE, true},
		{8, 1, INTERPOLATION_FAST, true},
		{7, 3, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{5, 3, INTERPOLATION_NONE, true},
		{5, 1, INTERPOLATION_FAST, true},
		{10, 3, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{7, 3, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{5, 3, INTERPOLATION_NONE, true},
		{5, 1, INTERPOLATION_FAST, true},
		{12, 1, INTERPOLATION_NONE, true},
		{12, 1, INTERPOLATION_FAST, true},
		{10, 1, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{12, 1, INTERPOLATION_NONE, true},
		{12, 1, INTERPOLATION_FAST, true},
		{10, 1, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{15, 3, INTERPOLATION_NONE, true},
		{15, 1, INTERPOLATION_FAST, true},
		{12, 3, INTERPOLATION_NONE, true},
		{12, 1, INTERPOLATION_FAST, true},
		{10, 3, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{12, 3, INTERPOLATION_NONE, true},
		{12, 1, INTERPOLATION_FAST, true},
		{10, 3, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{7, 3, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{5, 3, INTERPOLATION_NONE, true},
		{5, 1, INTERPOLATION_FAST, true},
		{3, 3, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence another_lead_sequence2={another_lead_notes2, &phase5, &velocity3};
	MDLRS_Pattern another_lead_pattern2; MDLRS_Pattern_new(&another_lead_pattern2, 64, &another_lead_sequence2, 1);
	MDLRS_Pattern_none another_lead_pattern3; MDLRS_Pattern_none_new(&another_lead_pattern3, 640);
	MDLRS_Note_key another_lead_notes3[]=
	{
		{7, 1, INTERPOLATION_FAST, true},
		{3, 3, INTERPOLATION_NONE, true},
		{3, 1, INTERPOLATION_FAST, true},
		{5, 3, INTERPOLATION_NONE, true},
		{5, 1, INTERPOLATION_FAST, true},
		{12, 3, INTERPOLATION_NONE, true},
		{12, 1, INTERPOLATION_FAST, true},
		{10, 7, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{7, 3, INTERPOLATION_NONE, true},
		{7, 1, INTERPOLATION_FAST, true},
		{5, 7, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence another_lead_sequence3={another_lead_notes3, &phase1, &velocity2};
	MDLRS_Pattern another_lead_pattern4; MDLRS_Pattern_new(&another_lead_pattern4, 32, &another_lead_sequence3, 1);
	MDLRS_Note_key another_lead_notes4[]=
	{
		{5, 1, INTERPOLATION_FAST, true},
		{8, 3, INTERPOLATION_NONE, true},
		{8, 1, INTERPOLATION_FAST, true},
		{3, 3, INTERPOLATION_NONE, true},
		{3, 1, INTERPOLATION_FAST, true},
		{10, 3, INTERPOLATION_NONE, true},
		{10, 1, INTERPOLATION_FAST, true},
		{3, 3, INTERPOLATION_NONE, true},
		{3, 1, INTERPOLATION_FAST, true},
		{12, 3, INTERPOLATION_NONE, true},
		{12, 1, INTERPOLATION_FAST, true},
		{3, 3, INTERPOLATION_NONE, true},
		{3, 1, INTERPOLATION_FAST, true},
		{14, 3, INTERPOLATION_NONE, true},
		{14, 1, INTERPOLATION_FAST, true},
		{3, 3, INTERPOLATION_NONE, true},
		{3, 1, INTERPOLATION_FAST, true},
		{15, 15, INTERPOLATION_NONE, true},
		{15, 2, INTERPOLATION_SMOOTH, true},
		{15.2f, 2, INTERPOLATION_SMOOTH, true},
		{14.6f, 2, INTERPOLATION_SMOOTH, true},
		{15.6f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SMOOTH, true},
		{14.2f, 2, INTERPOLATION_SMOOTH, true},
		{15.8f, 2, INTERPOLATION_SLOW, true},
		{3, 12, .pressed=false}
	};
	MDLRS_Sequence another_lead_sequence4={another_lead_notes4, &phase5, &velocity3};
	MDLRS_Pattern another_lead_pattern5; MDLRS_Pattern_new(&another_lead_pattern5, 112, &another_lead_sequence4, 1);
	MDLRS_Any_pattern *another_lead_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern9, (MDLRS_Any_pattern *)&another_lead_pattern1, (MDLRS_Any_pattern *)&another_lead_pattern2, (MDLRS_Any_pattern *)&another_lead_pattern3, (MDLRS_Any_pattern *)&another_lead_pattern4, (MDLRS_Any_pattern *)&pattern4, (MDLRS_Any_pattern *)&another_lead_pattern1, (MDLRS_Any_pattern *)&another_lead_pattern2, (MDLRS_Any_pattern *)&another_lead_pattern1, (MDLRS_Any_pattern *)&another_lead_pattern5, NULL};
	MDLRS_Any_pattern **another_lead_track=another_lead_patterns_sequence1;
	MDLRS_Discrete_key kick_phases1[]={{0, 8}, {0, 8}, {0, 10}, {0, 2}, {0, 4}};
	MDLRS_Sequence kick_sequence1={&note2, kick_phases1, &velocity2};
	MDLRS_Pattern kick_pattern1; MDLRS_Pattern_new(&kick_pattern1, 32, &kick_sequence1, 1);
	MDLRS_Discrete_key kick_phases2[]={{0, 20}, {0, 12}};
	MDLRS_Sequence kick_sequence2={&note2, kick_phases2, &velocity2};
	MDLRS_Pattern kick_pattern2; MDLRS_Pattern_new(&kick_pattern2, 32, &kick_sequence2, 1);
	MDLRS_Discrete_key kick_phases3[]={{0, 12}, {0, 8}, {0, 12}};
	MDLRS_Sequence kick_sequence3={&note2, kick_phases3, &velocity2};
	MDLRS_Pattern kick_pattern3; MDLRS_Pattern_new(&kick_pattern3, 32, &kick_sequence3, 1);
	MDLRS_Discrete_key kick_phases4[]={{0, 4}, {0, 2}, {0, 6}, {0, 8}, {0, 10}, {0, 2}};
	MDLRS_Sequence kick_sequence4={&note2, kick_phases4, &velocity2};
	MDLRS_Pattern kick_pattern4; MDLRS_Pattern_new(&kick_pattern4, 32, &kick_sequence4, 1);
	MDLRS_Note_key kick_notes1[]=
	{
		{.duration=4, .pressed=false},
		{0, 28, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence kick_sequence5={kick_notes1, kick_phases3, &velocity2};
	MDLRS_Pattern kick_pattern5; MDLRS_Pattern_new(&kick_pattern5, 32, &kick_sequence5, 1);
	MDLRS_Note_key kick_notes2[]=
	{
		{12, 16, INTERPOLATION_LINEAR, true},
		{0}
	};
	MDLRS_Discrete_key kick_phases5[]={{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}};
	MDLRS_Continuous_key kick_velocities1[]=
	{
		{0, 16, INTERPOLATION_LINEAR},
		{.5f}
	};
	MDLRS_Sequence kick_sequence6={kick_notes2, kick_phases5, kick_velocities1};
	MDLRS_Pattern kick_pattern6; MDLRS_Pattern_new(&kick_pattern6, 16, &kick_sequence6, 1);
	MDLRS_Pattern_none kick_pattern7; MDLRS_Pattern_none_new(&kick_pattern7, 384);
	MDLRS_Discrete_key kick_phases6[]={{0, 8}, {0, 16}, {0, 2}, {0, 2}, {0, 2}, {0, 2}};
	MDLRS_Sequence kick_sequence7={&note2, kick_phases6, &velocity2};
	MDLRS_Pattern kick_pattern8; MDLRS_Pattern_new(&kick_pattern8, 32, &kick_sequence7, 1);
	MDLRS_Discrete_key kick_phases7[]={{0, 8}, {0, 8}, {0, 8}, {0, 4}, {0, 2}, {0, 2}};
	MDLRS_Sequence kick_sequence8={&note2, kick_phases7, &velocity2};
	MDLRS_Pattern kick_pattern9; MDLRS_Pattern_new(&kick_pattern9, 32, &kick_sequence8, 1);
	MDLRS_Discrete_key kick_phases8[]={{0, 8}, {0, 8}, {0, 4}, {0, 4}, {0, 4}, {0, 4}};
	MDLRS_Sequence kick_sequence9={&note2, kick_phases8, &velocity2};
	MDLRS_Pattern kick_pattern10; MDLRS_Pattern_new(&kick_pattern10, 32, &kick_sequence9, 1);
	MDLRS_Discrete_key kick_phases9[]={{0, 14}, {0, 6}, {0, 8}, {0, 2}, {0, 2}};
	MDLRS_Sequence kick_sequence10={kick_notes1, kick_phases9, &velocity2};
	MDLRS_Pattern kick_pattern11; MDLRS_Pattern_new(&kick_pattern11, 32, &kick_sequence10, 1);
	MDLRS_Discrete_key kick_phases10[]={{0, 14}, {0, 6}, {0, 8}, {0, 4}};
	MDLRS_Sequence kick_sequence11={kick_notes1, kick_phases10, &velocity2};
	MDLRS_Pattern kick_pattern12; MDLRS_Pattern_new(&kick_pattern12, 32, &kick_sequence11, 1);
	MDLRS_Discrete_key kick_phases11[]={{0, 6}, {0, 6}, {0, 2}, {0, 6}, {0, 2}, {0, 10}};
	MDLRS_Sequence kick_sequence12={kick_notes1, kick_phases11, &velocity2};
	MDLRS_Pattern kick_pattern13; MDLRS_Pattern_new(&kick_pattern13, 32, &kick_sequence12, 1);
	MDLRS_Pattern_none kick_pattern14; MDLRS_Pattern_none_new(&kick_pattern14, 48);
	MDLRS_Any_pattern *kick_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern8, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&kick_pattern1, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&kick_pattern2, (MDLRS_Any_pattern *)&kick_pattern3, (MDLRS_Any_pattern *)&kick_pattern2, (MDLRS_Any_pattern *)&kick_pattern4, (MDLRS_Any_pattern *)&kick_pattern5, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern13, (MDLRS_Any_pattern *)&kick_pattern6, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&kick_pattern7, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&kick_pattern8, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&kick_pattern9, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&kick_pattern10, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&kick_pattern11, (MDLRS_Any_pattern *)&kick_pattern12, (MDLRS_Any_pattern *)&kick_pattern13, (MDLRS_Any_pattern *)&pattern12, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&kick_pattern14, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern12, NULL};
	MDLRS_Any_pattern **kick_track=kick_patterns_sequence1;
	MDLRS_Pattern_none toms_pattern1; MDLRS_Pattern_none_new(&toms_pattern1, 864);
	MDLRS_Note_key tom1_notes1[]=
	{
		{.duration=20, .pressed=false},
		{7, 12, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key tom2_notes1[]=
	{
		{.duration=22, .pressed=false},
		{0, 10, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence tom1_sequence1={tom1_notes1, &phase1, &velocity2};
	MDLRS_Sequence tom2_sequence1={tom2_notes1, &phase1, &velocity2};
	MDLRS_Pattern tom1_pattern1; MDLRS_Pattern_new(&tom1_pattern1, 32, &tom1_sequence1, 1);
	MDLRS_Pattern tom2_pattern1; MDLRS_Pattern_new(&tom2_pattern1, 32, &tom2_sequence1, 1);
	MDLRS_Note_key tom1_notes2[]=
	{
		{.duration=16, .pressed=false},
		{7, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key tom2_notes2[]=
	{
		{.duration=20, .pressed=false},
		{0, 12, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key tom1_phases1[]={{0, 18}, {0, 14}};
	MDLRS_Discrete_key tom2_phases1[]={{0, 22}, {0, 10}};
	MDLRS_Sequence tom1_sequence2={tom1_notes2, tom1_phases1, &velocity2};
	MDLRS_Sequence tom2_sequence2={tom2_notes2, tom2_phases1, &velocity2};
	MDLRS_Pattern tom1_pattern2; MDLRS_Pattern_new(&tom1_pattern2, 32, &tom1_sequence2, 1);
	MDLRS_Pattern tom2_pattern2; MDLRS_Pattern_new(&tom2_pattern2, 32, &tom2_sequence2, 1);
	MDLRS_Pattern_none tom1_pattern3; MDLRS_Pattern_none_new(&tom1_pattern3, 384);
	MDLRS_Note_key tom1_notes3[]=
	{
		{.duration=28, .pressed=false},
		{7, 4, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key tom1_phases2[]={{0, 30}, {0, 2}};
	MDLRS_Sequence tom1_sequence3={tom1_notes3, tom1_phases2, &velocity2};
	MDLRS_Pattern tom1_pattern4; MDLRS_Pattern_new(&tom1_pattern4, 32, &tom1_sequence3, 1);
	MDLRS_Pattern_none tom2_pattern3; MDLRS_Pattern_none_new(&tom2_pattern3, 480);
	MDLRS_Note_key tom1_notes4[]=
	{
		{.duration=8, .pressed=false},
		{7, 24, INTERPOLATION_NONE, true}
	};
	MDLRS_Note_key tom2_notes3[]=
	{
		{.duration=16, .pressed=false},
		{0, 16, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key tom1_phases3[]={{0, 10}, {0, 22}};
	MDLRS_Discrete_key tom2_phases2[]={{0, 18}, {0, 14}};
	MDLRS_Sequence tom1_sequence4={tom1_notes4, tom1_phases3, &velocity2};
	MDLRS_Sequence tom2_sequence3={tom2_notes3, tom2_phases2, &velocity2};
	MDLRS_Pattern tom1_pattern5; MDLRS_Pattern_new(&tom1_pattern5, 32, &tom1_sequence4, 1);
	MDLRS_Pattern tom2_pattern4; MDLRS_Pattern_new(&tom2_pattern4, 32, &tom2_sequence3, 1);
	MDLRS_Note_key tom2_notes4[]=
	{
		{.duration=30, .pressed=false},
		{0, 2, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence tom1_sequence5={tom1_notes3, &phase1, &velocity2};
	MDLRS_Sequence tom2_sequence4={tom2_notes4, &phase1, &velocity2};
	MDLRS_Pattern tom1_pattern6; MDLRS_Pattern_new(&tom1_pattern6, 32, &tom1_sequence5, 1);
	MDLRS_Pattern tom2_pattern5; MDLRS_Pattern_new(&tom2_pattern5, 32, &tom2_sequence4, 1);
	MDLRS_Any_pattern *tom1_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern4, (MDLRS_Any_pattern *)&tom1_pattern1, (MDLRS_Any_pattern *)&toms_pattern1, (MDLRS_Any_pattern *)&tom1_pattern2, (MDLRS_Any_pattern *)&tom1_pattern3, (MDLRS_Any_pattern *)&tom1_pattern4, (MDLRS_Any_pattern *)&pattern11, (MDLRS_Any_pattern *)&tom1_pattern5, (MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&tom1_pattern6, (MDLRS_Any_pattern *)&pattern12, NULL};
	MDLRS_Any_pattern *tom2_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern4, (MDLRS_Any_pattern *)&tom2_pattern1, (MDLRS_Any_pattern *)&toms_pattern1, (MDLRS_Any_pattern *)&tom2_pattern2, (MDLRS_Any_pattern *)&tom2_pattern3, (MDLRS_Any_pattern *)&tom2_pattern4, (MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&tom2_pattern5, (MDLRS_Any_pattern *)&pattern12, NULL};
	MDLRS_Any_pattern **tom1_track=tom1_patterns_sequence1;
	MDLRS_Any_pattern **tom2_track=tom2_patterns_sequence1;
	MDLRS_Discrete_key snare_phases1[]={{0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}, {0, 4}};
	MDLRS_Sequence snare_sequence1={&note2, snare_phases1, &velocity2};
	MDLRS_Pattern snare_pattern2; MDLRS_Pattern_new(&snare_pattern2, 32, &snare_sequence1, 1);
	MDLRS_Note_key snare_notes1[]=
	{
		{0, 16, INTERPOLATION_LINEAR, true},
		{12, 2, .pressed=false},
		{0, 14, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key snare_phases2[]={{0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 10}, {0, 6}, {0, 2}};
	MDLRS_Sequence snare_sequence2={snare_notes1, snare_phases2, &velocity2};
	MDLRS_Pattern snare_pattern3; MDLRS_Pattern_new(&snare_pattern3, 32, &snare_sequence2, 1);
	MDLRS_Pattern_none snare_pattern4; MDLRS_Pattern_none_new(&snare_pattern4, 512);
	MDLRS_Pattern snare_pattern5; MDLRS_Pattern_new(&snare_pattern5, 16, &snare_sequence2, 1);
	MDLRS_Discrete_key snare_phases3[]={{0, 28}, {0, 4}};
	MDLRS_Sequence snare_sequence3={&note2, snare_phases3, &velocity2};
	MDLRS_Pattern snare_pattern6; MDLRS_Pattern_new(&snare_pattern6, 32, &snare_sequence3, 1);
	MDLRS_Discrete_key snare_phases4[]={{0, 16}, {0, 4}, {0, 4}, {0, 4}, {0, 4}};
	MDLRS_Sequence snare_sequence4={notes3, snare_phases4, &velocity2};
	MDLRS_Pattern snare_pattern7; MDLRS_Pattern_new(&snare_pattern7, 32, &snare_sequence4, 1);
	MDLRS_Note_key snare_notes2[]=
	{
		{.duration=6, .pressed=false},
		{0, 26, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key snare_phases5[]={{0, 12}, {0, 10}, {0, 6}, {0, 4}};
	MDLRS_Sequence snare_sequence5={snare_notes2, snare_phases5, &velocity2};
	MDLRS_Pattern snare_pattern8; MDLRS_Pattern_new(&snare_pattern8, 32, &snare_sequence5, 1);
	MDLRS_Discrete_key snare_phases6[]={{0, 12}, {0, 8}, {0, 8}, {0, 4}};
	MDLRS_Sequence snare_sequence6={snare_notes2, snare_phases6, &velocity2};
	MDLRS_Pattern snare_pattern9; MDLRS_Pattern_new(&snare_pattern9, 32, &snare_sequence6, 1);
	MDLRS_Discrete_key snare_phases7[]={{0, 12}, {0, 6}, {0, 4}, {0, 10}};
	MDLRS_Sequence snare_sequence7={snare_notes2, snare_phases7, &velocity2};
	MDLRS_Pattern snare_pattern10; MDLRS_Pattern_new(&snare_pattern10, 32, &snare_sequence7, 1);
	MDLRS_Discrete_key snare_phases8[]={{0, 8}, {0, 8}, {0, 2}, {0, 6}, {0, 8}};
	MDLRS_Sequence snare_sequence8={&note2, snare_phases8, &velocity2};
	MDLRS_Pattern snare_pattern11; MDLRS_Pattern_new(&snare_pattern11, 32, &snare_sequence8, 1);
	MDLRS_Discrete_key snare_phases9[]={{0, 2}, {0, 22}, {0, 8}};
	MDLRS_Sequence snare_sequence9={&note2, snare_phases9, &velocity2};
	MDLRS_Pattern snare_pattern12; MDLRS_Pattern_new(&snare_pattern12, 32, &snare_sequence9, 1);
	MDLRS_Any_pattern *snare_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern5, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&snare_pattern2, (MDLRS_Any_pattern *)&snare_pattern3, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&snare_pattern4, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&snare_pattern2, (MDLRS_Any_pattern *)&snare_pattern5, (MDLRS_Any_pattern *)&pattern12, (MDLRS_Any_pattern *)&snare_pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&snare_pattern7, (MDLRS_Any_pattern *)&snare_pattern8, (MDLRS_Any_pattern *)&snare_pattern9, (MDLRS_Any_pattern *)&snare_pattern8, (MDLRS_Any_pattern *)&snare_pattern10, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&snare_pattern11, (MDLRS_Any_pattern *)&snare_pattern12, (MDLRS_Any_pattern *)&pattern11, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern12, NULL};
	MDLRS_Any_pattern **snare_track=snare_patterns_sequence1;
	MDLRS_Note_key hihat_notes1[]=
	{
		{.duration=2, .pressed=false},
		{0, 30, INTERPOLATION_NONE, true}
	};
	MDLRS_Discrete_key hihat_phases1[]={{0, 4}, {0, 6}, {0, 2}, {0, 6}, {0, 2}, {0, 6}, {0, 2}, {0, 4}};
	MDLRS_Continuous_key hihat_velocities1[]=
	{
		{.5f, 4, INTERPOLATION_NONE},
		{1, 6, INTERPOLATION_NONE},
		{.5f, 2, INTERPOLATION_NONE},
		{1, 6, INTERPOLATION_NONE},
		{.5f, 2, INTERPOLATION_NONE},
		{1, 6, INTERPOLATION_NONE},
		{.5f, 2, INTERPOLATION_NONE},
		{1, 4, INTERPOLATION_NONE}
	};
	MDLRS_Sequence hihat_sequence1={hihat_notes1, hihat_phases1, hihat_velocities1};
	MDLRS_Pattern hihat_pattern1; MDLRS_Pattern_new(&hihat_pattern1, 32, &hihat_sequence1, 1);
	MDLRS_Pattern hihat_pattern2; MDLRS_Pattern_new(&hihat_pattern2, 16, &hihat_sequence1, 1);
	MDLRS_Note_key hihat_notes2[]=
	{
		{.duration=4, .pressed=false},
		{0, 28, INTERPOLATION_NONE, true}
	};
	MDLRS_Sequence hihat_sequence2={hihat_notes2, phases6, &velocity2};
	MDLRS_Pattern hihat_pattern3; MDLRS_Pattern_new(&hihat_pattern3, 32, &hihat_sequence2, 1);
	MDLRS_Pattern_none hihat_pattern4; MDLRS_Pattern_none_new(&hihat_pattern4, 176);
	MDLRS_Any_pattern *hihat_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern7, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&pattern10, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&hihat_pattern1, (MDLRS_Any_pattern *)&hihat_pattern2, (MDLRS_Any_pattern *)&pattern12, (MDLRS_Any_pattern *)&hihat_pattern3, (MDLRS_Any_pattern *)&hihat_pattern3, (MDLRS_Any_pattern *)&hihat_pattern3, (MDLRS_Any_pattern *)&hihat_pattern3, (MDLRS_Any_pattern *)&hihat_pattern4, NULL};
	MDLRS_Any_pattern **hihat_track=hihat_patterns_sequence1;
	MDLRS_Sequence ride_sequence1={notes3, phases3, &velocity2};
	MDLRS_Pattern ride_pattern1; MDLRS_Pattern_new(&ride_pattern1, 32, &ride_sequence1, 1);
	MDLRS_Pattern_none ride_pattern2; MDLRS_Pattern_none_new(&ride_pattern2, 544);
	MDLRS_Discrete_key ride_phases1[]={{0, 16}, {0, 16}};
	MDLRS_Sequence ride_sequence2={notes3, ride_phases1, &velocity2};
	MDLRS_Pattern ride_pattern3; MDLRS_Pattern_new(&ride_pattern3, 32, &ride_sequence2, 1);
	MDLRS_Pattern_none ride_pattern4; MDLRS_Pattern_none_new(&ride_pattern4, 112);
	MDLRS_Any_pattern *ride_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern5, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&ride_pattern1, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&ride_pattern1, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&ride_pattern2, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern11, (MDLRS_Any_pattern *)&ride_pattern1, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&ride_pattern1, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&kick_pattern10, (MDLRS_Any_pattern *)&ride_pattern1, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&pattern3, (MDLRS_Any_pattern *)&ride_pattern3, (MDLRS_Any_pattern *)&ride_pattern3, (MDLRS_Any_pattern *)&ride_pattern4, NULL};
	MDLRS_Any_pattern **ride_track=ride_patterns_sequence1;
	MDLRS_Pattern_none crash_pattern1; MDLRS_Pattern_none_new(&crash_pattern1, 480);
	MDLRS_Sequence crash_sequence1={&note2, phases4, &velocity2};
	MDLRS_Pattern crash_pattern2; MDLRS_Pattern_new(&crash_pattern2, 32, &crash_sequence1, 1);
	MDLRS_Pattern_none crash_pattern3; MDLRS_Pattern_none_new(&crash_pattern3, 48);
	MDLRS_Any_pattern *crash_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern4, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern8, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern8, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern8, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&crash_pattern1, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern8, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern8, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern8, (MDLRS_Any_pattern *)&crash_pattern2, (MDLRS_Any_pattern *)&crash_pattern2, (MDLRS_Any_pattern *)&crash_pattern3, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&crash_pattern3, (MDLRS_Any_pattern *)&pattern2, (MDLRS_Any_pattern *)&pattern12, NULL};
	MDLRS_Any_pattern **crash_track=crash_patterns_sequence1;
	MDLRS_Pattern_none claps_pattern1; MDLRS_Pattern_none_new(&claps_pattern1, 704);
	MDLRS_Pattern_none claps_pattern2; MDLRS_Pattern_none_new(&claps_pattern2, 624);
	MDLRS_Discrete_key left_clap_phases1[]={{0, 16}, {0, 16}};
	MDLRS_Sequence left_clap_sequence1={&note2, left_clap_phases1, &velocity2};
	MDLRS_Pattern left_clap_pattern1; MDLRS_Pattern_new(&left_clap_pattern1, 32, &left_clap_sequence1, 1);
	MDLRS_Any_pattern *left_clap_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&claps_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&left_clap_pattern1, (MDLRS_Any_pattern *)&claps_pattern2, NULL};
	MDLRS_Any_pattern *right_clap_patterns_sequence1[]={(MDLRS_Any_pattern *)&pattern1, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&claps_pattern1, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&pattern6, (MDLRS_Any_pattern *)&claps_pattern2, NULL};
	MDLRS_Any_pattern **left_clap_track=left_clap_patterns_sequence1;
	MDLRS_Any_pattern **right_clap_track=right_clap_patterns_sequence1;
	MDLRS_Note_key sidechain_note1={-INFINITY, 256, INTERPOLATION_NONE, true};
	MDLRS_Sequence sidechain_sequence1={&sidechain_note1, &phase1, &velocity2};
	MDLRS_Pattern sidechain_pattern1; MDLRS_Pattern_new(&sidechain_pattern1, 32, &sidechain_sequence1, 1);
	MDLRS_Continuous_key sidechain_velocities1[]=
	{
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE}
	};
	MDLRS_Sequence sidechain_sequence2={&sidechain_note1, &phase1, sidechain_velocities1};
	MDLRS_Pattern sidechain_pattern2; MDLRS_Pattern_new(&sidechain_pattern2, 32, &sidechain_sequence2, 1);
	MDLRS_Continuous_key sidechain_velocities2[]=
	{
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 14, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 6, INTERPOLATION_NONE}
	};
	MDLRS_Sequence sidechain_sequence3={&sidechain_note1, &phase1, sidechain_velocities2};
	MDLRS_Pattern sidechain_pattern3; MDLRS_Pattern_new(&sidechain_pattern3, 32, &sidechain_sequence3, 1);
	MDLRS_Continuous_key sidechain_velocities3[]=
	{
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 6, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 6, INTERPOLATION_NONE}
	};
	MDLRS_Sequence sidechain_sequence4={&sidechain_note1, &phase1, sidechain_velocities3};
	MDLRS_Pattern sidechain_pattern4; MDLRS_Pattern_new(&sidechain_pattern4, 32, &sidechain_sequence4, 1);
	MDLRS_Continuous_key sidechain_velocities4[]=
	{
		{sidechain_depth, 4, INTERPOLATION_SMOOTH},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(4*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 0, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 4, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 0, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 6, INTERPOLATION_NONE}
		
	};
	MDLRS_Sequence sidechain_sequence5={&sidechain_note1, &phase5, sidechain_velocities4};
	MDLRS_Pattern sidechain_pattern5; MDLRS_Pattern_new(&sidechain_pattern5, 64, &sidechain_sequence5, 1);
	MDLRS_Continuous_key sidechain_velocities5[]=
	{
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1}
	};
	MDLRS_Sequence sidechain_sequence6={&sidechain_note1, &phase1, sidechain_velocities5};
	MDLRS_Pattern sidechain_pattern6; MDLRS_Pattern_new(&sidechain_pattern6, 6, &sidechain_sequence6, 1);
	MDLRS_Sequence sidechain_sequence7={&sidechain_note1, &phase5, &velocity3};
	MDLRS_Pattern sidechain_pattern7; MDLRS_Pattern_new(&sidechain_pattern7, 122, &sidechain_sequence7, 1);
	MDLRS_Discrete_key sidechain_phase1={0, 250};
	MDLRS_Sequence sidechain_sequence8={&sidechain_note1, &sidechain_phase1, &velocity4};
	MDLRS_Pattern sidechain_pattern8; MDLRS_Pattern_new(&sidechain_pattern8, 250, &sidechain_sequence8, 1);
	MDLRS_Continuous_key sidechain_velocities6[]=
	{
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 10, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2}
	};
	MDLRS_Sequence sidechain_sequence9={&sidechain_note1, &phase1, sidechain_velocities6};
	MDLRS_Pattern sidechain_pattern9; MDLRS_Pattern_new(&sidechain_pattern9, 32, &sidechain_sequence9, 1);
	MDLRS_Pattern sidechain_pattern10; MDLRS_Pattern_new(&sidechain_pattern10, 26, &sidechain_sequence1, 1);
	MDLRS_Continuous_key sidechain_velocities7[]=
	{
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 4, INTERPOLATION_SMOOTH},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(4*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2}
	};
	MDLRS_Sequence sidechain_sequence10={&sidechain_note1, &phase1, sidechain_velocities7};
	MDLRS_Pattern sidechain_pattern11; MDLRS_Pattern_new(&sidechain_pattern11, 32, &sidechain_sequence10, 1);
	MDLRS_Continuous_key sidechain_velocities8[]=
	{
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 4, INTERPOLATION_SMOOTH},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(4*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 4, INTERPOLATION_SMOOTH},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(4*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 4, INTERPOLATION_SMOOTH},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(4*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 4, INTERPOLATION_SMOOTH},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(4*(float)M_PI/6))/2}
	};
	MDLRS_Sequence sidechain_sequence11={&sidechain_note1, &phase1, sidechain_velocities8};
	MDLRS_Pattern sidechain_pattern12; MDLRS_Pattern_new(&sidechain_pattern12, 32, &sidechain_sequence11, 1);
	MDLRS_Continuous_key sidechain_velocities9[]=
	{
		{1, 4, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 4, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 0, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 0, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 4, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 0, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 2, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 0, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 0, INTERPOLATION_NONE},
		{sidechain_depth, 2, INTERPOLATION_SLOW},
		{sidechain_depth+(1-sidechain_depth)*(1-cosf(2*(float)M_PI/6))/2, 0, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 20, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 42, INTERPOLATION_NONE},
		{sidechain_depth, 6, INTERPOLATION_SMOOTH},
		{1, 10, INTERPOLATION_NONE}
	};
	MDLRS_Sequence sidechain_sequence12={&sidechain_note1, &sidechain_phase1, sidechain_velocities9};
	MDLRS_Pattern sidechain_pattern13; MDLRS_Pattern_new(&sidechain_pattern13, 176, &sidechain_sequence12, 1);
	MDLRS_Any_pattern *sidechain_patterns_sequence1[]={(MDLRS_Any_pattern *)&sidechain_pattern1, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern3, (MDLRS_Any_pattern *)&sidechain_pattern4, (MDLRS_Any_pattern *)&sidechain_pattern3, (MDLRS_Any_pattern *)&sidechain_pattern5, (MDLRS_Any_pattern *)&sidechain_pattern6, (MDLRS_Any_pattern *)&sidechain_pattern7, (MDLRS_Any_pattern *)&sidechain_pattern6, (MDLRS_Any_pattern *)&sidechain_pattern8, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern9, (MDLRS_Any_pattern *)&sidechain_pattern6, (MDLRS_Any_pattern *)&sidechain_pattern10, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern11, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern12, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern2, (MDLRS_Any_pattern *)&sidechain_pattern13, NULL};
	MDLRS_Any_pattern **sidechain_track=sidechain_patterns_sequence1;
	MDLRS_Sequencer_add(&song.supersaw_bass_part, &supersaw_bass_track, 1);
	MDLRS_Sequencer_add(&song.jumpy_bass_part, &jumpy_bass_track, 1);
	MDLRS_Sequencer_add(&song.piano_part, piano_tracks, 2);
	MDLRS_Sequencer_add(&song.chords_part, &chords_track, 1);
	MDLRS_Sequencer_add(&song.main_lead_part_single, &main_lead_single_track, 1);
	MDLRS_Sequencer_add(&song.main_lead_part, &main_lead_track, 1);
	MDLRS_Sequencer_add(&song.another_lead_part, &another_lead_track, 1);
	MDLRS_Sequencer_add(&song.kick_part, &kick_track, 1);
	MDLRS_Sequencer_add(&song.tom1_part, &tom1_track, 1);
	MDLRS_Sequencer_add(&song.tom2_part, &tom2_track, 1);
	MDLRS_Sequencer_add(&song.snare_part, &snare_track, 1);
	MDLRS_Sequencer_add(&song.hihat_part, &hihat_track, 1);
	MDLRS_Sequencer_add(&song.ride_part, &ride_track, 1);
	MDLRS_Sequencer_add(&song.crash_part, &crash_track, 1);
	MDLRS_Sequencer_add(&song.left_clap_part, &left_clap_track, 1);
	MDLRS_Sequencer_add(&song.right_clap_part, &right_clap_track, 1);
	MDLRS_Sequencer_add(&song.sidechain_part, &sidechain_track, 1);
	Future_sound_play(&song);
	if (output)
	{
		render_song(&song);
		Future_sound_remove(&song);
		return 0;
	}
	#if !defined(NOGUI)
		if (nogui)
		{
	#endif
		SDL_AudioSpec settings={sample_rate, AUDIO_S16, 2, 0, buffer_size, 0, 0, (SDL_AudioCallback)audi, &song};
		SDL_OpenAudio(&settings, NULL);
		SDL_PauseAudio(0);
		printf(PLAYER_TEXT);
		scanf("%s", NULL);
		SDL_CloseAudio();
		Future_sound_remove(&song);
		return 0;
	#if !defined(NOGUI)
		}
		TTF_Init();
		SDL_Window *window=SDL_CreateWindow("Серый MLGamer - Модулярис тест (0.0.0pre-alpha ремикс)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
		SDL_GLContext context=SDL_GL_CreateContext(window);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GLfloat matrix[]=
		{
			2.f/WINDOW_WIDTH, 0, 0, 0,
			0, -2.f/WINDOW_HEIGHT, 0, 0,
			0, 0, 1, 0,
			-1, 1, 0, 1
		};
		glLoadMatrixf(matrix);
		glEnableClientState(GL_VERTEX_ARRAY);
		GLuint cover_texture, logo_texture, gpl_texture;
		GLuint title_gray_texture, title_red_texture, title_green_texture, title_white_texture, G_texture, description_texture;
		SDL_Surface *cover=IMG_Load("share/Серый MLGamer - Модулярис тест.png");
		SDL_Surface *logo=IMG_Load("../share/logo ru_RU.png");
		SDL_Surface *gpl=IMG_Load("../share/gplv3-with-text-136x68.png");
		SDL_Surface *title_gray=render_text("Серый MLGamer - Модулярис тест (0.0.0pre-alpha ремикс)", 24, WINDOW_WIDTH-2*BUTTON_MARGIN, &title_gray_texture);
		SDL_Surface *title_red=render_text("Серый MLGamer - Модулярис тест", 24, WINDOW_WIDTH-2*BUTTON_MARGIN, &title_red_texture);
		SDL_Surface *title_green=render_text("Серый  LGamer - Модулярис", 24, WINDOW_WIDTH-2*BUTTON_MARGIN, &title_green_texture);
		SDL_Surface *title_white=render_text("Серый   Gamer -", 24, WINDOW_WIDTH-2*BUTTON_MARGIN, &title_white_texture);
		SDL_Surface *G=render_text("        G", 24, 0, &G_texture);
		SDL_Surface *description=render_text(
			"Это музыкальное произведение написано на языке программирования C на основе новейшей версии свободного каркаса Модулярис Ядро - 0.0.0pre-alpha! Это часть серии ремиксов \"гимна\" тестирования Модуляриса.\n"
			"Наконец-то я доработал функционал каркаса до того, что с её помощью теперь возможно сделать какую-то стоящую музыку. Но, конечно, есть ещё к чему стремиться. Недостаток функционала синтеза звука скомпенсирован сэмплами. Вообще хотел сделать из этого \"гимна\" что-то типа House-попсни, но опять же не хватило звуков, поэтому Happy Hardcore и мешанина звуков, чтобы базовые волноформы сильно не выпирали.\n"
			"Если трек проигрывается прерывисто, купите новый компьютер. Либо примите участие в оптимизации производительности каркаса на GitHub: https://github.com/Seriy-MLGamer/Modularis_Core\n"
			#if defined(__linux__)
				"Вы можете переключиться в консольный режим, введя в консоли:\n"
				"$ ./modularis-test --nogui\n"
				"Также вы можете скомпилировать проигрыватель без графического интерфейса. Он не будет требовать всяких дополнительных библиотек, только основную библиотеку SDL2 и, конечно, Модулярис Ядро. Введите команду:\n"
				"$ ./test-nogui\n", 14,
			#elif defined(_WIN32)
				"Вы можете скомпилировать консольный проигрыватель. Он не будет требовать всяких дополнительных библиотек, только основную библиотеку SDL2 и, конечно, Модулярис Ядро. Введите команду:\n"
				">test-nogui\n", 16,
			#endif
			WINDOW_WIDTH-2*BUTTON_MARGIN, &description_texture);
		cover_texture=new_texture(cover, GL_RGB);
		logo_texture=new_texture(logo, GL_RGBA);
		gpl_texture=new_texture(gpl, GL_RGBA);
		bool cursor_cough=false;
		float cursor_position;
		float cursor_old_position;
		GLint cursor_position_px;
		GLint cursor_x;
		Play play; Play_new(&play, (WINDOW_WIDTH-BUTTON_SIZE)/2, COVER_SIZE+CURSOR_LINE_WIDTH+BUTTON_MARGIN, BUTTON_SIZE, BUTTON_SIZE);
		Forward forward; Forward_new(&forward, play.x+BUTTON_SIZE+BUTTON_MARGIN, play.y, BUTTON_SIZE, BUTTON_SIZE);
		Backward backward; Backward_new(&backward, play.x-BUTTON_SIZE-BUTTON_MARGIN, play.y, BUTTON_SIZE, BUTTON_SIZE);
		Loop loop; Loop_new(&loop, forward.x+BUTTON_SIZE+BUTTON_MARGIN, forward.y, 2*BUTTON_SIZE/3, 2*BUTTON_SIZE/3);
		Info info; Info_new(&info, BUTTON_MARGIN, backward.y, 2*BUTTON_SIZE, BUTTON_SIZE);
		Render render; Render_new(&render, WINDOW_WIDTH-2*BUTTON_SIZE-BUTTON_MARGIN, loop.y, 2*BUTTON_SIZE, BUTTON_SIZE);
		Ok ok; Ok_new(&ok, WINDOW_WIDTH-BUTTON_MARGIN-2*BUTTON_SIZE, WINDOW_HEIGHT/2+24+BUTTON_MARGIN, 2*BUTTON_SIZE, BUTTON_SIZE);
		Cancel cancel; Cancel_new(&cancel, BUTTON_MARGIN, ok.y, 2*BUTTON_SIZE, BUTTON_SIZE);
		SDL_AudioSpec settings={sample_rate, AUDIO_S16, 2, 0, buffer_size, 0, 0, (SDL_AudioCallback)audi, &song};
		SDL_OpenAudio(&settings, NULL);
		SDL_PauseAudio(0);
		int scene=0;
		bool start_frame=true;
		uint32_t frame_time=0;
		uint32_t visualizer_position=0;
		int32_t show_time=0;
		char output_string[256]="out.wav";
		output=output_string;
		int string_position=7;
		Uint64 clocks=SDL_GetPerformanceCounter();
		while (true)
		{
			switch (scene)
			{
				case 0:
					if (frame_time>=(uint32_t)(40000*60/TEMPO))
					{
						glColor3ub(0, 0, 0);
						square(0, 0, WINDOW_WIDTH, COVER_SIZE);
						glColor3ub(0, 255, 0);
						for (int a=0; a!=WINDOW_WIDTH-1; a++) line(a, COVER_SIZE/2-COVER_SIZE*(int32_t)song.visualizer_buffer[(visualizer_position+visualizer_size*a/WINDOW_WIDTH)%visualizer_buffer_size]/65536, a+1, COVER_SIZE/2-COVER_SIZE*(int32_t)song.visualizer_buffer[(visualizer_position+visualizer_size*(a+1)/WINDOW_WIDTH)%visualizer_buffer_size]/65536, VISUALIZER_LINE_WIDTH, false, true);
					}
					if (start_frame)
					{
						if (frame_time<(uint32_t)(40000*60/TEMPO))
						{
							glBindTexture(GL_TEXTURE_2D, cover_texture);
							glColor3ub(256*frame_time/(uint32_t)(40000*60/TEMPO), 256*frame_time/(uint32_t)(40000*60/TEMPO), 256*frame_time/(uint32_t)(40000*60/TEMPO));
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(0, 0);
								glTexCoord2i(1, 0); glVertex2i(COVER_SIZE, 0);
								glTexCoord2i(0, 1); glVertex2i(0, COVER_SIZE);
								glTexCoord2i(1, 1); glVertex2i(COVER_SIZE, COVER_SIZE);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, 0);
						}
						else if (frame_time<(uint32_t)(40000*60/TEMPO)+5000)
						{
							glColor4ub(255, 255, 255, 256-256*(frame_time-(uint32_t)(40000*60/TEMPO))/5000);
							square(0, 0, COVER_SIZE, COVER_SIZE);
						}
						else start_frame=false;
					}
					if (show_time>0)
					{
						if (show_time<10000)
						{
							glColor4ub(0, 0, 0, 192*show_time/10000);
							square(0, 0, WINDOW_WIDTH, COVER_SIZE);
							glBindTexture(GL_TEXTURE_2D, title_gray_texture);
							glColor3ub(128, 128, 128);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(WINDOW_WIDTH-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN);
								glTexCoord2i(1, 0); glVertex2i(WINDOW_WIDTH+title_gray->w-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN);
								glTexCoord2i(0, 1); glVertex2i(WINDOW_WIDTH-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN+title_gray->h);
								glTexCoord2i(1, 1); glVertex2i(WINDOW_WIDTH+title_gray->w-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN+title_gray->h);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, title_red_texture);
							glColor3ub(255, 0, 0);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(WINDOW_WIDTH-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN);
								glTexCoord2i(1, 0); glVertex2i(WINDOW_WIDTH+title_red->w-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN);
								glTexCoord2i(0, 1); glVertex2i(WINDOW_WIDTH-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN+title_red->h);
								glTexCoord2i(1, 1); glVertex2i(WINDOW_WIDTH+title_red->w-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN+title_red->h);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, title_green_texture);
							glColor3ub(0, 255, 0);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(WINDOW_WIDTH-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN);
								glTexCoord2i(1, 0); glVertex2i(WINDOW_WIDTH+title_green->w-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN);
								glTexCoord2i(0, 1); glVertex2i(WINDOW_WIDTH-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN+title_green->h);
								glTexCoord2i(1, 1); glVertex2i(WINDOW_WIDTH+title_green->w-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN+title_green->h);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, title_white_texture);
							glColor3ub(255, 255, 255);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(WINDOW_WIDTH-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN);
								glTexCoord2i(1, 0); glVertex2i(WINDOW_WIDTH+title_white->w-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN);
								glTexCoord2i(0, 1); glVertex2i(WINDOW_WIDTH-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN+title_white->h);
								glTexCoord2i(1, 1); glVertex2i(WINDOW_WIDTH+title_white->w-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN+title_white->h);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, G_texture);
							glColor3ub(0, 0, 255);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(WINDOW_WIDTH-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN);
								glTexCoord2i(1, 0); glVertex2i(WINDOW_WIDTH+G->w-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN);
								glTexCoord2i(0, 1); glVertex2i(WINDOW_WIDTH-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN+G->h);
								glTexCoord2i(1, 1); glVertex2i(WINDOW_WIDTH+G->w-(WINDOW_WIDTH-BUTTON_MARGIN)*show_time/10000, BUTTON_MARGIN+G->h);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, description_texture);
							glColor3ub(255, 255, 255);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(BUTTON_MARGIN, COVER_SIZE-(BUTTON_MARGIN+description->h)*show_time/10000);
								glTexCoord2i(1, 0); glVertex2i(BUTTON_MARGIN+description->w, COVER_SIZE-(BUTTON_MARGIN+description->h)*show_time/10000);
								glTexCoord2i(0, 1); glVertex2i(BUTTON_MARGIN, COVER_SIZE+description->h-(BUTTON_MARGIN+description->h)*show_time/10000);
								glTexCoord2i(1, 1); glVertex2i(BUTTON_MARGIN+description->w, COVER_SIZE+description->h-(BUTTON_MARGIN+description->h)*show_time/10000);
							glEnd();
							glColor4ub(255, 255, 255, 256*show_time/10000);
						}
						else
						{
							glColor4ub(0, 0, 0, 192);
							square(0, 0, WINDOW_WIDTH, COVER_SIZE);
							glBindTexture(GL_TEXTURE_2D, title_gray_texture);
							glColor3ub(128, 128, 128);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN);
								glTexCoord2i(1, 0); glVertex2i(BUTTON_MARGIN+title_gray->w, BUTTON_MARGIN);
								glTexCoord2i(0, 1); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN+title_gray->h);
								glTexCoord2i(1, 1); glVertex2i(BUTTON_MARGIN+title_gray->w, BUTTON_MARGIN+title_gray->h);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, title_red_texture);
							glColor3ub(255, 0, 0);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN);
								glTexCoord2i(1, 0); glVertex2i(BUTTON_MARGIN+title_red->w, BUTTON_MARGIN);
								glTexCoord2i(0, 1); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN+title_red->h);
								glTexCoord2i(1, 1); glVertex2i(BUTTON_MARGIN+title_red->w, BUTTON_MARGIN+title_red->h);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, title_green_texture);
							glColor3ub(0, 255, 0);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN);
								glTexCoord2i(1, 0); glVertex2i(BUTTON_MARGIN+title_green->w, BUTTON_MARGIN);
								glTexCoord2i(0, 1); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN+title_green->h);
								glTexCoord2i(1, 1); glVertex2i(BUTTON_MARGIN+title_green->w, BUTTON_MARGIN+title_green->h);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, title_white_texture);
							glColor3ub(255, 255, 255);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN);
								glTexCoord2i(1, 0); glVertex2i(BUTTON_MARGIN+title_white->w, BUTTON_MARGIN);
								glTexCoord2i(0, 1); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN+title_white->h);
								glTexCoord2i(1, 1); glVertex2i(BUTTON_MARGIN+title_white->w, BUTTON_MARGIN+title_white->h);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, G_texture);
							glColor3ub(0, 0, 255);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN);
								glTexCoord2i(1, 0); glVertex2i(BUTTON_MARGIN+G->w, BUTTON_MARGIN);
								glTexCoord2i(0, 1); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN+G->h);
								glTexCoord2i(1, 1); glVertex2i(BUTTON_MARGIN+G->w, BUTTON_MARGIN+G->h);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, description_texture);
							glColor3ub(255, 255, 255);
							glBegin(GL_TRIANGLE_STRIP);
								glTexCoord2i(0, 0); glVertex2i(BUTTON_MARGIN, COVER_SIZE-(BUTTON_MARGIN+description->h));
								glTexCoord2i(1, 0); glVertex2i(BUTTON_MARGIN+description->w, COVER_SIZE-(BUTTON_MARGIN+description->h));
								glTexCoord2i(0, 1); glVertex2i(BUTTON_MARGIN, COVER_SIZE-BUTTON_MARGIN);
								glTexCoord2i(1, 1); glVertex2i(BUTTON_MARGIN+description->w, COVER_SIZE-BUTTON_MARGIN);
							glEnd();
						}
						glBindTexture(GL_TEXTURE_2D, logo_texture);
						glBegin(GL_TRIANGLE_STRIP);
							glTexCoord2i(0, 0); glVertex2i(BUTTON_MARGIN, BUTTON_MARGIN+title_gray->h);
							glTexCoord2i(1, 0); glVertex2i(BUTTON_MARGIN+logo->w*(COVER_SIZE-(2*BUTTON_MARGIN+description->h+title_gray->h))/logo->h, BUTTON_MARGIN+title_gray->h);
							glTexCoord2i(0, 1); glVertex2i(BUTTON_MARGIN, COVER_SIZE-(BUTTON_MARGIN+description->h));
							glTexCoord2i(1, 1); glVertex2i(BUTTON_MARGIN+logo->w*(COVER_SIZE-(2*BUTTON_MARGIN+description->h+title_gray->h))/logo->h, COVER_SIZE-(BUTTON_MARGIN+description->h));
						glEnd();
						glBindTexture(GL_TEXTURE_2D, gpl_texture);
						glBegin(GL_TRIANGLE_STRIP);
							glTexCoord2i(0, 0); glVertex2i(WINDOW_WIDTH-(BUTTON_MARGIN+gpl->w), title_gray->h+(COVER_SIZE-gpl->h-(description->h+title_gray->h))/2);
							glTexCoord2i(1, 0); glVertex2i(WINDOW_WIDTH-BUTTON_MARGIN, title_gray->h+(COVER_SIZE-gpl->h-(description->h+title_gray->h))/2);
							glTexCoord2i(0, 1); glVertex2i(WINDOW_WIDTH-(BUTTON_MARGIN+gpl->w), title_gray->h+(COVER_SIZE+gpl->h-(description->h+title_gray->h))/2);
							glTexCoord2i(1, 1); glVertex2i(WINDOW_WIDTH-BUTTON_MARGIN, title_gray->h+(COVER_SIZE+gpl->h-(description->h+title_gray->h))/2);
						glEnd();
						glBindTexture(GL_TEXTURE_2D, 0);
					}
					glColor3ub(0, 0, 0);
					square(0, COVER_SIZE, WINDOW_WIDTH, PANEL_SIZE);
					if (!cursor_cough) cursor_position=song.sidechain_part.cursor_position.value;
					glColor3ub(255, 255, 255);
					square(0, COVER_SIZE, (GLint)(WINDOW_WIDTH*cursor_position)/SONG_LENGTH, CURSOR_LINE_WIDTH);
					glBegin(GL_LINES);
						glVertex2i(0, COVER_SIZE+1);
						glVertex2i(WINDOW_WIDTH, COVER_SIZE+1);
						glVertex2i(0, COVER_SIZE+CURSOR_LINE_WIDTH);
						glVertex2i(WINDOW_WIDTH, COVER_SIZE+CURSOR_LINE_WIDTH);
					glEnd();
					cursor_position_px=(GLint)((WINDOW_WIDTH-CURSOR_WIDTH)*cursor_position)/SONG_LENGTH;
					if (cursor_cough) glColor3ub(0, 128, 0);
					else
					{
						glColor3ub(0, 255, 0);
						if (play.play) if (!song.sidechain_part.play.value)
						{
							Future_sound_pause(&song);
							play.play=false;
						}
					}
					line(cursor_position_px+CURSOR_WIDTH/2, COVER_SIZE, cursor_position_px+CURSOR_WIDTH/2, COVER_SIZE+CURSOR_LINE_WIDTH, CURSOR_WIDTH, true, true);
					Play_draw(&play);
					Forward_draw(&forward);
					Backward_draw(&backward);
					Loop_draw(&loop);
					Info_draw(&info);
					Render_draw(&render);
					SDL_GL_SwapWindow(window);
					SDL_Event event;
					while (SDL_PollEvent(&event)) switch (event.type)
					{
						case SDL_QUIT:
							quit:
							SDL_CloseAudio();
							Info_remove(&info);
							Render_remove(&render);
							Ok_remove(&ok);
							Cancel_remove(&cancel);
							glDeleteTextures(1, &cover_texture);
							glDeleteTextures(1, &logo_texture);
							glDeleteTextures(1, &gpl_texture);
							glDeleteTextures(1, &title_gray_texture);
							glDeleteTextures(1, &title_red_texture);
							glDeleteTextures(1, &title_green_texture);
							glDeleteTextures(1, &title_white_texture);
							glDeleteTextures(1, &G_texture);
							glDeleteTextures(1, &description_texture);
							SDL_FreeSurface(cover);
							SDL_FreeSurface(logo);
							SDL_FreeSurface(gpl);
							SDL_FreeSurface(title_gray);
							SDL_FreeSurface(title_red);
							SDL_FreeSurface(title_green);
							SDL_FreeSurface(title_white);
							SDL_FreeSurface(G);
							SDL_FreeSurface(description);
							SDL_GL_DeleteContext(context);
							SDL_DestroyWindow(window);
							TTF_Quit();
							Future_sound_remove(&song);
							return 0;
						case SDL_MOUSEMOTION:
							Button_hover((Button *)&play, &event.motion);
							Button_hover((Button *)&forward, &event.motion);
							Button_hover((Button *)&backward, &event.motion);
							Button_hover((Button *)&loop, &event.motion);
							Button_hover((Button *)&info, &event.motion);
							Button_hover((Button *)&render, &event.motion);
							if (cursor_cough)
							{
								cursor_position=cursor_old_position+SONG_LENGTH*(event.motion.x-cursor_x)/(WINDOW_WIDTH-CURSOR_WIDTH);
								cursor_position=(cursor_position>=0&&cursor_position<=SONG_LENGTH)*cursor_position+(cursor_position>SONG_LENGTH)*SONG_LENGTH;
							}
							break;
						case SDL_MOUSEBUTTONDOWN:
							Button_press((Button *)&play, &event.button);
							Button_press((Button *)&forward, &event.button);
							Button_press((Button *)&backward, &event.button);
							Button_press((Button *)&loop, &event.button);
							Button_press((Button *)&info, &event.button);
							Button_press((Button *)&render, &event.button);
							if (!cursor_cough) if (event.button.button==1) if (event.button.x>=cursor_position_px&&event.button.x<cursor_position_px+CURSOR_WIDTH&&(event.button.y>=COVER_SIZE&&event.button.y<COVER_SIZE+CURSOR_LINE_WIDTH||sqrtf(powf(event.button.x-(cursor_position_px+CURSOR_WIDTH/2), 2)+powf(event.button.y-COVER_SIZE, 2))<=CURSOR_WIDTH/2.f||sqrtf(powf(event.button.x-(cursor_position_px+CURSOR_WIDTH/2), 2)+powf(event.button.y-(COVER_SIZE+CURSOR_LINE_WIDTH), 2))<=CURSOR_WIDTH/2.f))
							{
								if (play.play) Future_sound_pause(&song);
								cursor_cough=true;
								cursor_old_position=cursor_position;
								cursor_x=event.button.x;
							}
							break;
						case SDL_MOUSEBUTTONUP:
							Button_release((Button *)&play, &event.button, &song);
							Button_release((Button *)&forward, &event.button, &song);
							Button_release((Button *)&backward, &event.button, &song);
							Button_release((Button *)&loop, &event.button, &song);
							Button_release((Button *)&info, &event.button, &song);
							Button_release((Button *)&render, &event.button, &song);
							if (cursor_cough) if (event.button.button==1)
							{
								Future_sound_set_position(&song, cursor_position);
								if (play.play) Future_sound_play(&song);
								cursor_cough=false;
							}
							break;
						case SDL_KEYDOWN:
							if (event.key.keysym.sym==SDLK_SPACE) Button_key_press((Button *)&play, &song);
							if (event.key.keysym.sym==SDLK_RIGHT) Button_key_press((Button *)&forward, &song);
							if (event.key.keysym.sym==SDLK_LEFT) Button_key_press((Button *)&backward, &song);
							break;
						case SDL_KEYUP:
							if (event.key.keysym.sym==SDLK_SPACE) Button_key_release((Button *)&play);
							if (event.key.keysym.sym==SDLK_RIGHT) Button_key_release((Button *)&forward);
							if (event.key.keysym.sym==SDLK_LEFT) Button_key_release((Button *)&backward);
					}
					if (render.render)
					{
						start_frame=false;
						frame_time=40000*60/TEMPO;
						show_time=0;
						Ok_remove(&ok);
						Cancel_remove(&cancel);
						Ok_new(&ok, ok.x, ok.y, ok.width, ok.height);
						Cancel_new(&cancel, cancel.x, cancel.y, cancel.width, cancel.height);
						SDL_PauseAudio(1);
						scene=1;
						break;
					}
					Uint64 diff_clock=SDL_GetPerformanceCounter()-clocks;
					Uint64 frequency=SDL_GetPerformanceFrequency();
					if (song.loaded)
					{
						visualizer_position=song.visualizer_position;
						song.loaded=false;
					}
					else visualizer_position=(visualizer_position+sample_rate*diff_clock/frequency)%visualizer_buffer_size;
					if (start_frame) frame_time+=10000*diff_clock/frequency;
					if (info.show)
					{
						if (show_time<10000) show_time+=10000*diff_clock/frequency;
					}
					else if (show_time>0) show_time-=10000*diff_clock/frequency;
					clocks+=diff_clock;
					break;
				case 1:
					glColor3ub(0, 0, 0);
					square(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
					glColor3ub(255, 255, 255);
					glBegin(GL_LINES);
						glVertex2i(0, WINDOW_HEIGHT/2+1);
						glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT/2+1);
						glVertex2i(0, WINDOW_HEIGHT/2+24);
						glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT/2+24);
					glEnd();
					if (strlen(output_string))
					{
						GLuint output_texture;
						SDL_Surface *output_surface=render_text(output_string, 16, 0, &output_texture);
						glBindTexture(GL_TEXTURE_2D, output_texture);
						glBegin(GL_TRIANGLE_STRIP);
							glTexCoord2i(0, 0); glVertex2i(0, WINDOW_HEIGHT/2+2);
							glTexCoord2i(1, 0); glVertex2i(output_surface->w, WINDOW_HEIGHT/2+2);
							glTexCoord2i(0, 1); glVertex2i(0, WINDOW_HEIGHT/2+2+output_surface->h);
							glTexCoord2i(1, 1); glVertex2i(output_surface->w, WINDOW_HEIGHT/2+2+output_surface->h);
						glEnd();
						glBindTexture(GL_TEXTURE_2D, 0);
						glDeleteTextures(1, &output_texture);
						SDL_FreeSurface(output_surface);
					}
					Ok_draw(&ok);
					Cancel_draw(&cancel);
					SDL_GL_SwapWindow(window);
					SDL_Delay(50);
					while (SDL_PollEvent(&event)) switch (event.type)
					{
						case SDL_QUIT: goto quit;
						case SDL_MOUSEMOTION:
							Button_hover((Button *)&ok, &event.motion);
							Button_hover((Button *)&cancel, &event.motion);
							break;
						case SDL_MOUSEBUTTONDOWN:
							Button_press((Button *)&ok, &event.button);
							Button_press((Button *)&cancel, &event.button);
							break;
						case SDL_MOUSEBUTTONUP:
							Button_release((Button *)&ok, &event.button, &song);
							Button_release((Button *)&cancel, &event.button, &song);
							break;
						case SDL_KEYDOWN:
							switch (event.key.keysym.sym)
							{
								case SDLK_BACKSPACE:
									if (string_position) for (int a=--string_position; a!=255; a++) output_string[a]=output_string[a+1];
									break;
								case SDLK_DELETE:
									if (output_string[string_position]) for (int a=string_position; a!=255; a++) output_string[a]=output_string[a+1];
									break;
								case SDLK_LEFT:
									if (string_position) string_position--;
									break;
								case SDLK_RIGHT:
									if (output_string[string_position]) string_position++;
									break;
								case SDLK_RETURN:
									Ok_on_click(&ok, &song);
									break;
								case SDLK_ESCAPE:
									Cancel_on_click(&cancel, &song);
									break;
								default:
									for (int a=255; a!=string_position-1; a--) output_string[a+1]=output_string[a];
									output_string[string_position++]=event.key.keysym.sym;
							}
					}
					if (ok.ok||cancel.cancel)
					{
						Future_sound_remove(&song);
						Info_remove(&info);
						Render_remove(&render);
						Future_sound_new(&song);
						MDLRS_Sequencer_add(&song.supersaw_bass_part, &supersaw_bass_track, 1);
						MDLRS_Sequencer_add(&song.jumpy_bass_part, &jumpy_bass_track, 1);
						MDLRS_Sequencer_add(&song.piano_part, piano_tracks, 2);
						MDLRS_Sequencer_add(&song.chords_part, &chords_track, 1);
						MDLRS_Sequencer_add(&song.main_lead_part_single, &main_lead_single_track, 1);
						MDLRS_Sequencer_add(&song.main_lead_part, &main_lead_track, 1);
						MDLRS_Sequencer_add(&song.another_lead_part, &another_lead_track, 1);
						MDLRS_Sequencer_add(&song.kick_part, &kick_track, 1);
						MDLRS_Sequencer_add(&song.tom1_part, &tom1_track, 1);
						MDLRS_Sequencer_add(&song.tom2_part, &tom2_track, 1);
						MDLRS_Sequencer_add(&song.snare_part, &snare_track, 1);
						MDLRS_Sequencer_add(&song.hihat_part, &hihat_track, 1);
						MDLRS_Sequencer_add(&song.ride_part, &ride_track, 1);
						MDLRS_Sequencer_add(&song.crash_part, &crash_track, 1);
						MDLRS_Sequencer_add(&song.left_clap_part, &left_clap_track, 1);
						MDLRS_Sequencer_add(&song.right_clap_part, &right_clap_track, 1);
						MDLRS_Sequencer_add(&song.sidechain_part, &sidechain_track, 1);
						Play_new(&play, play.x, play.y, play.width, play.height);
						Forward_new(&forward, forward.x, forward.y, forward.width, forward.height);
						Backward_new(&backward, backward.x, backward.y, backward.width, backward.height);
						if (loop.loop) Loop_new(&loop, loop.x, loop.y, loop.width, loop.height);
						else
						{
							Loop_new(&loop, loop.x, loop.y, loop.width, loop.height);
							Loop_on_click(&loop, &song);
						}
						Info_new(&info, info.x, info.y, info.width, info.height);
						Render_new(&render, render.x, render.y, render.width, render.height);
						if (ok.ok)
						{
							Future_sound_play(&song);
							SDL_DetachThread(SDL_CreateThread((SDL_ThreadFunction)render_song, "render_song", &song));
							scene=2;
						}
						else
						{
							SDL_PauseAudio(0);
							visualizer_position=0;
							clocks=SDL_GetPerformanceCounter();
							scene=0;
						}
					}
					break;
				case 2:
					if (song.loaded)
					{
						Future_sound_remove(&song);
						Future_sound_new(&song);
						MDLRS_Sequencer_add(&song.supersaw_bass_part, &supersaw_bass_track, 1);
						MDLRS_Sequencer_add(&song.jumpy_bass_part, &jumpy_bass_track, 1);
						MDLRS_Sequencer_add(&song.piano_part, piano_tracks, 2);
						MDLRS_Sequencer_add(&song.chords_part, &chords_track, 1);
						MDLRS_Sequencer_add(&song.main_lead_part_single, &main_lead_single_track, 1);
						MDLRS_Sequencer_add(&song.main_lead_part, &main_lead_track, 1);
						MDLRS_Sequencer_add(&song.another_lead_part, &another_lead_track, 1);
						MDLRS_Sequencer_add(&song.kick_part, &kick_track, 1);
						MDLRS_Sequencer_add(&song.tom1_part, &tom1_track, 1);
						MDLRS_Sequencer_add(&song.tom2_part, &tom2_track, 1);
						MDLRS_Sequencer_add(&song.snare_part, &snare_track, 1);
						MDLRS_Sequencer_add(&song.hihat_part, &hihat_track, 1);
						MDLRS_Sequencer_add(&song.ride_part, &ride_track, 1);
						MDLRS_Sequencer_add(&song.crash_part, &crash_track, 1);
						MDLRS_Sequencer_add(&song.left_clap_part, &left_clap_track, 1);
						MDLRS_Sequencer_add(&song.right_clap_part, &right_clap_track, 1);
						MDLRS_Sequencer_add(&song.sidechain_part, &sidechain_track, 1);
						if (!loop.loop) Future_sound_unloop(&song);
						SDL_PauseAudio(0);
						visualizer_position=0;
						clocks=SDL_GetPerformanceCounter();
						scene=0;
						break;
					}
					glColor3ub(0, 255, 0);
					cursor_position=song.sidechain_part.cursor_position.value;
					square(0, 0, (GLint)(WINDOW_WIDTH*cursor_position)/SONG_LENGTH, WINDOW_HEIGHT);
					glColor3ub(0, 0, 0);
					square((GLint)(WINDOW_WIDTH*cursor_position)/SONG_LENGTH, 0, WINDOW_WIDTH-(GLint)(WINDOW_WIDTH*cursor_position)/SONG_LENGTH, WINDOW_HEIGHT);
					SDL_GL_SwapWindow(window);
					SDL_Delay(50);
			}
		}
	#endif
}
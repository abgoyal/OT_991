
#ifndef __SOUND_OPL3_H
#define __SOUND_OPL3_H


#include <sound/core.h>
#include <sound/hwdep.h>
#include <sound/timer.h>
#include <sound/seq_midi_emul.h>
#ifdef CONFIG_SND_SEQUENCER_OSS
#include <sound/seq_oss.h>
#include <sound/seq_oss_legacy.h>
#endif
#include <sound/seq_device.h>
#include <sound/asound_fm.h>


#define OPL3_REG_TEST			0x01
#define   OPL3_ENABLE_WAVE_SELECT	0x20

#define OPL3_REG_TIMER1			0x02
#define OPL3_REG_TIMER2			0x03
#define OPL3_REG_TIMER_CONTROL		0x04	/* Left side */
#define   OPL3_IRQ_RESET		0x80
#define   OPL3_TIMER1_MASK		0x40
#define   OPL3_TIMER2_MASK		0x20
#define   OPL3_TIMER1_START		0x01
#define   OPL3_TIMER2_START		0x02

#define OPL3_REG_CONNECTION_SELECT	0x04	/* Right side */
#define   OPL3_LEFT_4OP_0		0x01
#define   OPL3_LEFT_4OP_1		0x02
#define   OPL3_LEFT_4OP_2		0x04
#define   OPL3_RIGHT_4OP_0		0x08
#define   OPL3_RIGHT_4OP_1		0x10
#define   OPL3_RIGHT_4OP_2		0x20

#define OPL3_REG_MODE			0x05	/* Right side */
#define   OPL3_OPL3_ENABLE		0x01	/* OPL3 mode */
#define   OPL3_OPL4_ENABLE		0x02	/* OPL4 mode */

#define OPL3_REG_KBD_SPLIT		0x08	/* Left side */
#define   OPL3_COMPOSITE_SINE_WAVE_MODE	0x80	/* Don't use with OPL-3? */
#define   OPL3_KEYBOARD_SPLIT		0x40

#define OPL3_REG_PERCUSSION		0xbd	/* Left side only */
#define   OPL3_TREMOLO_DEPTH		0x80
#define   OPL3_VIBRATO_DEPTH		0x40
#define	  OPL3_PERCUSSION_ENABLE	0x20
#define   OPL3_BASSDRUM_ON		0x10
#define   OPL3_SNAREDRUM_ON		0x08
#define   OPL3_TOMTOM_ON		0x04
#define   OPL3_CYMBAL_ON		0x02
#define   OPL3_HIHAT_ON			0x01

#define OPL3_REG_AM_VIB			0x20
#define   OPL3_TREMOLO_ON		0x80
#define   OPL3_VIBRATO_ON		0x40
#define   OPL3_SUSTAIN_ON		0x20
#define   OPL3_KSR			0x10	/* Key scaling rate */
#define   OPL3_MULTIPLE_MASK		0x0f	/* Frequency multiplier */

 /*
  *   KSL/Total level (0x40 to 0x55)
  */
#define OPL3_REG_KSL_LEVEL		0x40
#define   OPL3_KSL_MASK			0xc0	/* Envelope scaling bits */
#define   OPL3_TOTAL_LEVEL_MASK		0x3f	/* Strength (volume) of OP */

#define OPL3_REG_ATTACK_DECAY		0x60
#define   OPL3_ATTACK_MASK		0xf0
#define   OPL3_DECAY_MASK		0x0f

#define OPL3_REG_SUSTAIN_RELEASE	0x80
#define   OPL3_SUSTAIN_MASK		0xf0
#define   OPL3_RELEASE_MASK		0x0f

#define OPL3_REG_WAVE_SELECT		0xe0
#define   OPL3_WAVE_SELECT_MASK		0x07

#define OPL3_REG_FNUM_LOW		0xa0

#define OPL3_REG_KEYON_BLOCK		0xb0
#define	  OPL3_KEYON_BIT		0x20
#define	  OPL3_BLOCKNUM_MASK		0x1c
#define   OPL3_FNUM_HIGH_MASK		0x03

#define OPL3_REG_FEEDBACK_CONNECTION	0xc0
#define   OPL3_FEEDBACK_MASK		0x0e	/* Valid just for 1st OP of a voice */
#define   OPL3_CONNECTION_BIT		0x01
#define   OPL3_STEREO_BITS		0x30	/* OPL-3 only */
#define     OPL3_VOICE_TO_LEFT		0x10
#define     OPL3_VOICE_TO_RIGHT		0x20


#define OPL3_LEFT		0x0000
#define OPL3_RIGHT		0x0100

#define OPL3_HW_AUTO		0x0000
#define OPL3_HW_OPL2		0x0200
#define OPL3_HW_OPL3		0x0300
#define OPL3_HW_OPL3_SV		0x0301	/* S3 SonicVibes */
#define OPL3_HW_OPL3_CS		0x0302	/* CS4232/CS4236+ */
#define OPL3_HW_OPL3_FM801	0x0303	/* FM801 */
#define OPL3_HW_OPL3_CS4281	0x0304	/* CS4281 */
#define OPL3_HW_OPL4		0x0400	/* YMF278B/YMF295 */
#define OPL3_HW_OPL4_ML		0x0401	/* YMF704/YMF721 */
#define OPL3_HW_MASK		0xff00

#define MAX_OPL2_VOICES		9
#define MAX_OPL3_VOICES		18

struct snd_opl3;


/* FM operator */
struct fm_operator {
	unsigned char am_vib;
	unsigned char ksl_level;
	unsigned char attack_decay;
	unsigned char sustain_release;
	unsigned char wave_select;
} __attribute__((packed));

/* Instrument data */
struct fm_instrument {
	struct fm_operator op[4];
	unsigned char feedback_connection[2];
	unsigned char echo_delay;
	unsigned char echo_atten;
	unsigned char chorus_spread;
	unsigned char trnsps;
	unsigned char fix_dur;
	unsigned char modes;
	unsigned char fix_key;
};

/* type */
#define FM_PATCH_OPL2	0x01		/* OPL2 2 operators FM instrument */
#define FM_PATCH_OPL3	0x02		/* OPL3 4 operators FM instrument */

/* Instrument record */
struct fm_patch {
	unsigned char prog;
	unsigned char bank;
	unsigned char type;
	struct fm_instrument inst;
	char name[24];
	struct fm_patch *next;
};


struct snd_opl3_voice {
	int  state;		/* status */
#define SNDRV_OPL3_ST_OFF		0	/* Not playing */
#define SNDRV_OPL3_ST_ON_2OP	1	/* 2op voice is allocated */
#define SNDRV_OPL3_ST_ON_4OP	2	/* 4op voice is allocated */
#define SNDRV_OPL3_ST_NOT_AVAIL	-1	/* voice is not available */

	unsigned int time;	/* An allocation time */
	unsigned char note;	/* Note currently assigned to this voice */

	unsigned long note_off;	/* note-off time */
	int note_off_check;	/* check note-off time */

	unsigned char keyon_reg;	/* KON register shadow */

	struct snd_midi_channel *chan;	/* Midi channel for this note */
};

struct snd_opl3 {
	unsigned long l_port;
	unsigned long r_port;
	struct resource *res_l_port;
	struct resource *res_r_port;
	unsigned short hardware;
	/* hardware access */
	void (*command) (struct snd_opl3 * opl3, unsigned short cmd, unsigned char val);
	unsigned short timer_enable;
	int seq_dev_num;	/* sequencer device number */
	struct snd_timer *timer1;
	struct snd_timer *timer2;
	spinlock_t timer_lock;

	void *private_data;
	void (*private_free)(struct snd_opl3 *);

	struct snd_hwdep *hwdep;
	spinlock_t reg_lock;
	struct snd_card *card;		/* The card that this belongs to */
	unsigned char fm_mode;		/* OPL mode, see SNDRV_DM_FM_MODE_XXX */
	unsigned char rhythm;		/* percussion mode flag */
	unsigned char max_voices;	/* max number of voices */
#if defined(CONFIG_SND_SEQUENCER) || defined(CONFIG_SND_SEQUENCER_MODULE)
#define SNDRV_OPL3_MODE_SYNTH 0		/* OSS - voices allocated by application */
#define SNDRV_OPL3_MODE_SEQ 1		/* ALSA - driver handles voice allocation */
	int synth_mode;			/* synth mode */
	int seq_client;

	struct snd_seq_device *seq_dev;	/* sequencer device */
	struct snd_midi_channel_set * chset;

#ifdef CONFIG_SND_SEQUENCER_OSS
	struct snd_seq_device *oss_seq_dev;	/* OSS sequencer device */
	struct snd_midi_channel_set * oss_chset;
#endif
 
#define OPL3_PATCH_HASH_SIZE	32
	struct fm_patch *patch_table[OPL3_PATCH_HASH_SIZE];

	struct snd_opl3_voice voices[MAX_OPL3_VOICES]; /* Voices (OPL3 'channel') */
	int use_time;			/* allocation counter */

	unsigned short connection_reg;	/* connection reg shadow */
	unsigned char drum_reg;		/* percussion reg shadow */

	spinlock_t voice_lock;		/* Lock for voice access */

	struct timer_list tlist;	/* timer for note-offs and effects */
	int sys_timer_status;		/* system timer run status */
	spinlock_t sys_timer_lock;	/* Lock for system timer access */
#endif
};

/* opl3.c */
void snd_opl3_interrupt(struct snd_hwdep * hw);
int snd_opl3_new(struct snd_card *card, unsigned short hardware,
		 struct snd_opl3 **ropl3);
int snd_opl3_init(struct snd_opl3 *opl3);
int snd_opl3_create(struct snd_card *card,
		    unsigned long l_port, unsigned long r_port,
		    unsigned short hardware,
		    int integrated,
		    struct snd_opl3 ** opl3);
int snd_opl3_timer_new(struct snd_opl3 * opl3, int timer1_dev, int timer2_dev);
int snd_opl3_hwdep_new(struct snd_opl3 * opl3, int device, int seq_device,
		       struct snd_hwdep ** rhwdep);

/* opl3_synth */
int snd_opl3_open(struct snd_hwdep * hw, struct file *file);
int snd_opl3_ioctl(struct snd_hwdep * hw, struct file *file,
		   unsigned int cmd, unsigned long arg);
int snd_opl3_release(struct snd_hwdep * hw, struct file *file);

void snd_opl3_reset(struct snd_opl3 * opl3);

#if defined(CONFIG_SND_SEQUENCER) || defined(CONFIG_SND_SEQUENCER_MODULE)
long snd_opl3_write(struct snd_hwdep *hw, const char __user *buf, long count,
		    loff_t *offset);
int snd_opl3_load_patch(struct snd_opl3 *opl3,
			int prog, int bank, int type,
			const char *name,
			const unsigned char *ext,
			const unsigned char *data);
struct fm_patch *snd_opl3_find_patch(struct snd_opl3 *opl3, int prog, int bank,
				     int create_patch);
void snd_opl3_clear_patches(struct snd_opl3 *opl3);
#else
#define snd_opl3_write	NULL
static inline void snd_opl3_clear_patches(struct snd_opl3 *opl3) {}
#endif

#endif /* __SOUND_OPL3_H */
